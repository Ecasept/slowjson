#include "json/deserialize.h"
#include "json/utils/string/file.h"
#include "jsontestsuite.h"
#include "fxx_test.h"
#include "jsonperf.h"
#include "json/utils/alloc/default.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

Result run_once(const char *filename) {
	string json;
	Result r = read_file_to_string(filename, &json);
	if (!r.success) return r;

	Parser parser = json_parser_new(config_default_parser_config());
	JSONDocument result = json_document_new();
	r = json_parser_deserialize(&parser, &json, &result);
	string_free(&json, ga);
	
	if (!r.success) {
		json_document_free(&result);
		return r;
	}

	// Successfully deserialized
	json_document_free(&result);
	return new_success();
}

Result run_string(string *json, JSONDocument *out_result) {
	Parser parser = json_parser_new(config_default_parser_config());
	JSONDocument result = json_document_new();
	Result r = json_parser_deserialize(&parser, json, &result);
	if (!r.success) {
		json_document_free(&result);
		return r;
	}

	if (out_result != NULL) {
		*out_result = result;
		return new_success();
	}

	json_document_free(&result);
	return new_success();
}
