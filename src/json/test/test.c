#include "../deserialize.h"
#include "../utils/string/file.h"
#include "jsontestsuite.h"
#include "fxx_test.h"
#include "jsonperf.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef RUN_TESTS
static int is_number(const char *value) {
	if (value == NULL || *value == '\0') return 0;
	for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; p++) {
		if (!isdigit(*p)) return 0;
	}
	return 1;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		// Run all tests
		run_jsontestsuite();
		run_fxx_test();
		run_jsonperf(1, NULL);
	} else {
		// Run a specific test
		const char *test_name = argv[1];
		if (strcmp(test_name, "jsontestsuite") == 0) {
			run_jsontestsuite();
		} else if (strcmp(test_name, "fxx_test") == 0) {
			run_fxx_test();
		} else if (strcmp(test_name, "jsonperf") == 0) {
			const char *benchmark_name = NULL;
			size_t iterations = 1;
			if (argc >= 3) {
				if (is_number(argv[2])) {
					iterations = (size_t)atoi(argv[2]);
					if (argc >= 4) {
						benchmark_name = argv[3];
					}
				} else {
					benchmark_name = argv[2];
					if (argc >= 4 && is_number(argv[3])) {
						iterations = (size_t)atoi(argv[3]);
					}
				}
			}
			run_jsonperf(iterations, benchmark_name);
		} else {
			printf("Unknown test: %s\n", test_name);
			return 1;
		}
	}
	return 0;
}
#endif

Result run_once(const char *filename) {
	string json;
	Result r = read_file_to_string(filename, &json);
	if (!r.success) return r;

	JSONValue root;
	r = json_deserialize(&json, &root, config_default_parser_config());
	string_free(&json);
	
	if (!r.success) return r;

	// Successfully deserialized
	json_value_free(&root);
	return new_success();
}

Result run_string(string *json, JSONValue *out_root) {
	JSONValue root;
	Result r = json_deserialize(json, &root, config_default_parser_config());
	if (r.success) {
		if (out_root != NULL) {
			*out_root = root;
		} else {
			json_value_free(&root);
		}
	}
	return r;
}
