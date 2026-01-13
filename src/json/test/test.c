#include "../deserialize.h"
#include "../utils/string/file.h"
#include "jsontestsuite.h"
#include "fxx_test.h"

#ifdef RUN_TESTS
int main(int argc, char **argv) {
	if (argc < 2) {
		// Run all tests
		run_jsontestsuite();
		run_fxx_test();
	} else {
		// Run a specific test
		const char *test_name = argv[1];
		if (strcmp(test_name, "jsontestsuite") == 0) {
			run_jsontestsuite();
		} else if (strcmp(test_name, "fxx_test") == 0) {
			run_fxx_test();
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
