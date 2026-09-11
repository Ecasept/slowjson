#include "json/deserialize.h"
#include "json/serialize.h"
#include "json/utils/alloc/default.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <threads.h>
#include <stdatomic.h>

static int read_number(const char *text, uint32_t *out) {
    char *end;
    if (*text < '0' || *text > '9') return 0;
    errno = 0;
    unsigned long value = strtoul(text, &end, 10);
    if (errno || *end || value == 0 || value > UINT32_MAX) return 0;
    *out = (uint32_t)value;
    return 1;
}

typedef struct {
    uint32_t total, completed;
    struct timespec started;
    mtx_t lock;
    atomic_bool failed;
} Progress;

typedef struct {
    uint32_t iterations, seed, index;
    size_t accepted, unicode_inputs;
    Progress *progress;
} Worker;

// Independent SplitMix64 state for each worker; no shared rand() lock.
static uint32_t random_next(uint64_t *state) {
    uint64_t z = (*state += UINT64_C(0x9e3779b97f4a7c15));
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return (uint32_t)((z ^ (z >> 31)) >> 32);
}

static void report_progress(Progress *progress, uint32_t count) {
    mtx_lock(&progress->lock);
    uint32_t previous = progress->completed;
    progress->completed += count;
    if (previous / 100000 != progress->completed / 100000) {
        struct timespec now;
        timespec_get(&now, TIME_UTC);
        double elapsed = difftime(now.tv_sec, progress->started.tv_sec) +
                         (now.tv_nsec - progress->started.tv_nsec) / 1e9;
        double eta = elapsed / progress->completed * (progress->total - progress->completed);
        printf("Progress: %" PRIu32 "/%" PRIu32 " (%.1f%%); ETA: %.0f seconds\n",
               progress->completed, progress->total,
               100.0 * progress->completed / progress->total, eta);
        fflush(stdout);
    }
    mtx_unlock(&progress->lock);
}

static int fuzz_worker(void *arg) {
    Worker *worker = arg;
    uint64_t random_state = worker->seed;
    uint32_t pending = 0;
    uchar buffer[100];
    string input = {.arr = {.data = buffer, .length = 0, .capacity = sizeof(buffer)}};
    JSONDocument doc = json_document_new();
    ParserConfig config = config_default_parser_config();
    config.double_overflow_behavior = CONFIG_DOUBLE_OVERFLOW_ERROR;
    config.exponent_overflow_behavior = CONFIG_EXPONENT_OVERFLOW_ERROR;
    // Finite numbers outside int64_t can still be represented as doubles.
    config.integer_overflow_behavior = CONFIG_INTEGER_OVERFLOW_DOUBLE_FALLBACK;
    Parser parser = json_parser_new(config);

    for (uint32_t i = 0; i < worker->iterations; i++) {
        if (atomic_load_explicit(&worker->progress->failed, memory_order_relaxed)) break;
        input.arr.length = 50 + random_next(&random_state) % 51;
        for (size_t j = 0; j < input.arr.length; j++) {
            // ASCII includes JSON syntax, whitespace, control bytes and nulls.
            buffer[j] = (uchar)(random_next(&random_state) % 128);
        }
        // Replace 1–4 bytes in 10% of inputs without growing the buffer.
        // Arbitrary high bytes can form valid UTF-8, lone continuations,
        // truncated sequences, overlong encodings, or illegal leading bytes.
        bool injected_unicode = random_next(&random_state) % 10 == 0;
        if (injected_unicode) {
            size_t length = 1 + random_next(&random_state) % 4;
            size_t offset = (size_t)random_next(&random_state) % (input.arr.length - length + 1);
            for (size_t j = 0; j < length; j++) {
                buffer[offset + j] = (uchar)(128 + random_next(&random_state) % 128);
            }
            worker->unicode_inputs++;
        }
        Result r = json_parser_deserialize(&parser, &input, &doc);
        if (r.success) {
            worker->accepted++;
            string output;
            r = json_document_serialize(&doc, &output);
            if (!r.success) {
                error_free(r);
            } else {
                bool matches = sv_eq(as_sv(input), as_sv(output));
                string_free(&output, ga);
                if (!matches) {
                    fprintf(stderr, "Serialized output differs from input at iteration %" PRIu32 ", seed=%" PRIu32 ", thread=%" PRIu32 "\n",
                            i + 1, worker->seed, worker->index);
                    atomic_store_explicit(&worker->progress->failed, true, memory_order_relaxed);
                    break;
                }
            }
        } else {
            error_free(r);
        }
        if (++pending == 1000) {
            report_progress(worker->progress, pending);
            pending = 0;
        }
    }
    if (pending) report_progress(worker->progress, pending);
    json_document_free(&doc);
    return 0;
}

int main(int argc, char **argv) {
    uint32_t iterations = 100000, seed = 1, thread_count = 1;
    if (argc > 4 || (argc > 1 && !read_number(argv[1], &iterations)) ||
        (argc > 2 && !read_number(argv[2], &seed)) ||
        (argc > 3 && !read_number(argv[3], &thread_count)) ||
        thread_count - 1 > UINT32_MAX - seed) {
        fprintf(stderr, "Usage: %s [iterations] [seed] [threads] (seed + threads - 1 must fit uint32)\n", argv[0]);
        return 2;
    }
    if (thread_count > iterations) thread_count = iterations;
    printf("Fuzzing: iterations=%" PRIu32 ", seed=%" PRIu32 ", threads=%" PRIu32 "\n",
           iterations, seed, thread_count);
    fflush(stdout);
    Progress progress = {.total = iterations};
    atomic_init(&progress.failed, false);
    if (mtx_init(&progress.lock, mtx_plain) != thrd_success) {
        fprintf(stderr, "Failed to initialize progress lock\n");
        return 1;
    }
    timespec_get(&progress.started, TIME_UTC);
    thrd_t *threads = calloc(thread_count, sizeof(*threads));
    Worker *workers = calloc(thread_count, sizeof(*workers));
    if (!threads || !workers) {
        fprintf(stderr, "Failed to allocate workers\n");
        free(threads);
        free(workers);
        mtx_destroy(&progress.lock);
        return 1;
    }
    uint32_t launched = 0;
    for (; launched < thread_count; launched++) {
        workers[launched] = (Worker){
            .iterations = iterations / thread_count + (launched < iterations % thread_count),
            .seed = seed + launched,
            .index = launched,
            .progress = &progress
        };
        if (thrd_create(&threads[launched], fuzz_worker, &workers[launched]) != thrd_success) {
            fprintf(stderr, "Failed to start thread %" PRIu32 "\n", launched);
            atomic_store(&progress.failed, true);
            break;
        }
    }
    size_t accepted = 0, unicode_inputs = 0;
    for (uint32_t i = 0; i < launched; i++) {
        thrd_join(threads[i], NULL);
        accepted += workers[i].accepted;
        unicode_inputs += workers[i].unicode_inputs;
    }
    int status = atomic_load(&progress.failed) ? 1 : 0;
    if (!status) {
        printf("Completed %" PRIu32 " iterations; %zu Unicode inputs; %zu raw inputs accepted.\n",
               progress.completed, unicode_inputs, accepted);
    }
    free(workers);
    free(threads);
    mtx_destroy(&progress.lock);
    return status;
}
