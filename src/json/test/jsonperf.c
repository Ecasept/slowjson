#include "jsonperf.h"
#include "../deserialize.h"
#include "../utils/string/file.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "../utils/alloc/default.h"

struct Test {
	const char *filename;
	const char *name;
};
typedef struct Test Test;

#define PERF_DIR "src/json/test/files/nativejson-benchmark/"
#define TEST(test_name)                                                        \
	{ .filename = PERF_DIR test_name ".json", .name = test_name }

static Test tests[] = {
	TEST("canada"),
	TEST("citm_catalog"),
	TEST("twitter"),
};

static const double GB_IN_BYTES = 1024.0 * 1024.0 * 1024.0;

static int run_perf_on_test(Test test, double *time_taken, double *gb_per_sec) {
	string json;
	Result r = read_file_to_string(test.filename, &json);
	if (!r.success) {
		string err_msg = format_error(r);
		printf("Failed to read file: %s (%.*s)\n", test.filename,
				(int)err_msg.arr.length, err_msg.arr.data);
		string_free(&err_msg, ga);
		return 0;
	}

	printf("Parsing %s (Size: %zu bytes)\n", test.name, json.arr.length);

	clock_t start = clock();
	Parser parser = json_parser_new(config_default_parser_config());
	ParserResult result;
	r = json_parser_deserialize(&parser, &json, &result);
	if (r.success) {
		parser_result_free(&result);
	}
	clock_t end = clock();

	if (!r.success) {
		string err_msg = format_error(r);
		printf("Failed to parse JSON: %.*s\n", (int)err_msg.arr.length, err_msg.arr.data);
		string_free(&err_msg, ga);
		string_free(&json, ga);
		error_free(r);
		return 0;
	}

	*time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Parsing %s took %f seconds\n", test.name, *time_taken);
	*gb_per_sec = ((double)json.arr.length / GB_IN_BYTES) / *time_taken;
	printf("Throughput: %f GB/s\n", *gb_per_sec);

	string_free(&json, ga);
	return 1;
}

static void list_available_tests(void) {
	size_t num_tests = sizeof(tests) / sizeof(tests[0]);
	printf("Available benchmarks:\n");
	for (size_t i = 0; i < num_tests; i++) {
		printf("- %s\n", tests[i].name);
	}
}

static int find_test_index(const char *benchmark_name) {
	if (benchmark_name == NULL) return -1;
	size_t num_tests = sizeof(tests) / sizeof(tests[0]);
	for (size_t i = 0; i < num_tests; i++) {
		if (strcmp(tests[i].name, benchmark_name) == 0) {
			return (int)i;
		}
	}
	return -1;
}

void run_jsonperf(size_t iterations, const char *benchmark_name) {
	int test_index = find_test_index(benchmark_name);
	if (benchmark_name != NULL && test_index < 0) {
		printf("Unknown benchmark: %s\n", benchmark_name);
		list_available_tests();
		return;
	}

	size_t num_tests = sizeof(tests) / sizeof(tests[0]);
	size_t tracked_tests = (test_index >= 0) ? 1 : num_tests;
	double total_time[sizeof(tests) / sizeof(tests[0])] = {0};
	double total_throughput[sizeof(tests) / sizeof(tests[0])] = {0};
	size_t success_counts[sizeof(tests) / sizeof(tests[0])] = {0};

	for (size_t i = 0; i < iterations; i++) {
		printf("=== Iteration %zu ===\n", i + 1);
		if (test_index >= 0) {
			double time_taken = 0.0;
			double gb_per_sec = 0.0;
			if (run_perf_on_test(tests[(size_t)test_index], &time_taken, &gb_per_sec)) {
				total_time[0] += time_taken;
				total_throughput[0] += gb_per_sec;
				success_counts[0] += 1;
			}
			printf("\n");
		} else {
			for (size_t j = 0; j < num_tests; j++) {
				double time_taken = 0.0;
				double gb_per_sec = 0.0;
				if (run_perf_on_test(tests[j], &time_taken, &gb_per_sec)) {
					total_time[j] += time_taken;
					total_throughput[j] += gb_per_sec;
					success_counts[j] += 1;
				}
				printf("\n");
			}
		}
	}

	printf("=== Averages ===\n");
	for (size_t i = 0; i < tracked_tests; i++) {
		const char *name = (test_index >= 0) ? tests[(size_t)test_index].name : tests[i].name;
		if (success_counts[i] == 0) {
			printf("%s: no successful runs\n", name);
			continue;
		}
		double avg_time = total_time[i] / (double)success_counts[i];
		double avg_throughput = total_throughput[i] / (double)success_counts[i];
		printf("%s avg time: %f seconds\n", name, avg_time);
		printf("%s avg throughput: %f GB/s\n", name, avg_throughput);
	}
}
