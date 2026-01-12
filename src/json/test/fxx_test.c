#include "../utils/string/dstring.h"
#include "../utils/string/file.h"
#include "../utils/string/string_view.h"
#include "../deserialize.h"
#include "../internal/jsonvalue.h"
#include "../config.h"
#include <inttypes.h>
#include <stdlib.h>

struct Test {
	const char *filename;
	const char *name;
};
typedef struct Test Test;

#define TEST_DIR "src/json/test/files/parse-number-fxx-test-data/data/"
#define TEST(test_name) { .filename = TEST_DIR test_name ".txt", .name = test_name }

Test fxx_tests[] = {
	TEST("exhaustive-float16"),
	TEST("freetype-2-7"),
	TEST("google-double-conversion"),
	TEST("google-wuffs"),
	TEST("ibm-fpgen"),
	TEST("lemire-fast-double-parser"),
	TEST("lemire-fast-float"),
	TEST("more-test-cases"),
	TEST("remyoudompheng-fptest-0"),
	TEST("remyoudompheng-fptest-1"),
	TEST("remyoudompheng-fptest-2"),
	TEST("remyoudompheng-fptest-3"),
	TEST("tencent-rapidjson"),
	TEST("ulfjack-ryu"),
};

// ...existing code...
static Result get_fxx_test_data(string *line, double *expected, string *json_input) {
	StringSplitIterator it;
	string_split_iterator_init(&it, as_sv(*line), ' ');

	bool has_part;
	int index = 0;

	uint64_t expected_bits = 0;
	
	bool found_hex = false;
	bool found_json = false;

	while (1) {
		Result r = string_split_iterator_next(&it, &has_part);
		if (!r.success) {
			return r;
		}
		if (!has_part) break;

		if (index == 2) {
			char buf[65];
			size_t len = it.current_part.size;
			if (len > 64) len = 64;
			memcpy(buf, it.current_part.data, len);
			buf[len] = '\0';
			expected_bits = strtoull(buf, NULL, 16);
			memcpy(expected, &expected_bits, sizeof(double));
			found_hex = true;
		} else if (index == 3) {
			string_from_view(json_input, it.current_part);
			found_json = true;
		}
		index++;
	}

	if (!found_hex || !found_json) {
		if (found_json) {
			string_free(json_input);
		}
		return new_error("Malformed fxx test line", EParserSyntaxError);
	}

	return new_success();
}

static Result run_single_fxx_test(double expected, string *json_input) {
	JSONValue val;
	ParserConfig config = config_default_parser_config();
	Result r = json_deserialize(json_input, &val, config);
	if (!r.success) {
		return r;
	}

	if (val.type != JSON_NUMBER) {
		json_value_free(&val);
		return new_errorf("Expected JSON number, got %s", EParserSyntaxError, jtostr(val.type));
	}

	double d;
	if (val.number.is_integer) {
		d = (double)val.number.int_value;
	} else {
		d = val.number.float_value;
	}
	uint64_t actual_bits, expected_bits;
	memcpy(&actual_bits, &d, sizeof(double));
	memcpy(&expected_bits, &expected, sizeof(double));
	
	if (actual_bits != expected_bits) {
		json_value_free(&val);
		return new_errorf("Mismatch: expected %lx, got %lx", EParserSyntaxError, expected_bits, actual_bits);
	}

	json_value_free(&val);
	return new_success();
}

Result run_fxx_test_file(const char *filename) {
	FileLineIterator iterator;
	Result r = read_lines(filename, &iterator);
	if (!r.success) return r;

	bool has_line;
	while (1) {
		r = file_line_iterator_next(&iterator, &has_line);
		if (!r.success) {
			file_line_iterator_close(&iterator);
			return r;
		}
		if (!has_line) break;

		// Process each line
		double expected;
		string json_input;

		r = get_fxx_test_data(&iterator.current_line, &expected, &json_input);
		if (!r.success) {
			file_line_iterator_close(&iterator);
			return r;
		}
		
		r = run_single_fxx_test(expected, &json_input);
		
		if (!r.success) {
			file_line_iterator_close(&iterator);
			error_prependf(&r, "For input: %.*s: ", (int)json_input.arr.length, json_input.arr.data);
			string_free(&json_input);
			return r;
		}
		string_free(&json_input);
	}

	file_line_iterator_close(&iterator);
	return new_success();
}

void run_fxx_test(void) {
	size_t test_count = sizeof(fxx_tests) / sizeof(Test);
	for (size_t i = 0; i < test_count; i++) {
		Test test = fxx_tests[i];
		Result r = run_fxx_test_file(test.filename);
		if (!r.success) {
			fprintf(stderr, "FXX Test '%s' failed: %s\n", test.name, r.message);
			exit(EXIT_FAILURE);
		} else {
			printf("FXX Test '%s' passed.\n", test.name);
		}
	}
}
