#include "../deserialize.h"
#include "../utils/string/file.h"
#include "jsontestsuite.h"
#include "fxx_test.h"

#ifdef RUN_TESTS
int main() {
	run_jsontestsuite();
	run_fxx_test();
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
