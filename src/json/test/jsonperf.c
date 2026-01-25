#include "jsonperf.h"
#include "../deserialize.h"
#include "../utils/string/file.h"
#include <time.h>
#include <stdio.h>

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

void run_perf_on_test(Test test) {
	string json;
	Result r = read_file_to_string(test.filename, &json);
	if (!r.success) {
		string err_msg = format_error(r);
		printf("Failed to read file: %s (%.*s)\n", test.filename,
				(int)err_msg.arr.length, err_msg.arr.data);
		string_free(&err_msg);
		return;
	}

	printf("Parsing %s (Size: %zu bytes)\n", test.name, json.arr.length);

	clock_t start = clock();
	JSONValue root;
	r = json_deserialize(&json, &root, config_default_parser_config());
	clock_t end = clock();

	if (!r.success) {
		string err_msg = format_error(r);
		printf("Failed to parse JSON: %.*s\n", (int)err_msg.arr.length, err_msg.arr.data);
		string_free(&err_msg);
		string_free(&json);
		error_free(r);
		return;
	}

	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Parsing %s took %f seconds\n", test.name, time_taken);
	double gb_per_sec = ((double)json.arr.length / GB_IN_BYTES) / time_taken;
	printf("Throughput: %f GB/s\n", gb_per_sec);

	json_value_free(&root);
	string_free(&json);
}

void run_jsonperf(size_t iterations) {
	for (size_t i = 0; i < iterations; i++) {
		printf("=== Iteration %zu ===\n", i + 1);
		size_t num_tests = sizeof(tests) / sizeof(tests[0]);
		for (size_t j = 0; j < num_tests; j++) {
			run_perf_on_test(tests[j]);
			printf("\n");
		}
	}
}
