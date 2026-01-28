#include <stdlib.h>
#include "test.h"
#include <stdio.h>
#include "../utils/alloc/default.h"

struct Test {
	const char *filename;
	const char *name;
};
typedef struct Test Test;

#define PARSER_DIR "src/json/test/files/jsontestsuite/test_parsing/"
#define TRANSFORM_DIR "src/json/test/files/jsontestsuite/test_transform/"
#define TESTP(test_name) { .filename = PARSER_DIR test_name ".json", .name = test_name }
#define TESTT(test_name) { .filename = TRANSFORM_DIR test_name ".json", .name = test_name }
/*

i_number_double_huge_neg_exp
i_number_huge_exp
i_number_neg_int_huge_exp
i_number_pos_double_huge_exp
i_number_real_neg_overflow
i_number_real_pos_overflow
i_number_real_underflow
i_number_too_big_neg_int
i_number_too_big_pos_int
i_number_very_big_negative_int
i_object_key_lone_2nd_surrogate
i_string_1st_surrogate_but_2nd_missing
i_string_1st_valid_surrogate_2nd_invalid
i_string_incomplete_surrogate_and_escape_valid
i_string_incomplete_surrogate_pair
i_string_incomplete_surrogates_escape_valid
i_string_invalid_lonely_surrogate
i_string_invalid_surrogate
i_string_invalid_utf-8
i_string_inverted_surrogates_U+1D11E
i_string_iso_latin_1
i_string_lone_second_surrogate
i_string_lone_utf8_continuation_byte
i_string_not_in_unicode_range
i_string_overlong_sequence_2_bytes
i_string_overlong_sequence_6_bytes
i_string_overlong_sequence_6_bytes_null
i_string_truncated-utf-8
i_string_utf16BE_no_BOM
i_string_utf16LE_no_BOM
i_string_UTF-16LE_with_BOM
i_string_UTF-8_invalid_sequence
i_string_UTF8_surrogate_U+D800
i_structure_500_nested_arrays
i_structure_UTF-8_BOM_empty_object



*/




Test n_tests[] = {
	TESTP("n_array_1_true_without_comma"),
	TESTP("n_array_a_invalid_utf8"),
	TESTP("n_array_colon_instead_of_comma"),
	TESTP("n_array_comma_after_close"),
	TESTP("n_array_comma_and_number"),
	TESTP("n_array_double_comma"),
	TESTP("n_array_double_extra_comma"),
	TESTP("n_array_extra_close"),
	TESTP("n_array_extra_comma"),
	TESTP("n_array_incomplete_invalid_value"),
	TESTP("n_array_incomplete"),
	TESTP("n_array_inner_array_no_comma"),
	TESTP("n_array_invalid_utf8"),
	TESTP("n_array_items_separated_by_semicolon"),
	TESTP("n_array_just_comma"),
	TESTP("n_array_just_minus"),
	TESTP("n_array_missing_value"),
	TESTP("n_array_newlines_unclosed"),
	TESTP("n_array_number_and_comma"),
	TESTP("n_array_number_and_several_commas"),
	TESTP("n_array_spaces_vertical_tab_formfeed"),
	TESTP("n_array_star_inside"),
	TESTP("n_array_unclosed"),
	TESTP("n_array_unclosed_trailing_comma"),
	TESTP("n_array_unclosed_with_new_lines"),
	TESTP("n_array_unclosed_with_object_inside"),
	TESTP("n_incomplete_false"),
	TESTP("n_incomplete_null"),
	TESTP("n_incomplete_true"),
	TESTP("n_multidigit_number_then_00"),
	TESTP("n_number_0.1.2"),
	TESTP("n_number_-01"),
	TESTP("n_number_0.3e+"),
	TESTP("n_number_0.3e"),
	TESTP("n_number_0_capital_E+"),
	TESTP("n_number_0_capital_E"),
	TESTP("n_number_0.e1"),
	TESTP("n_number_0e+"),
	TESTP("n_number_0e"),
	TESTP("n_number_1_000"),
	TESTP("n_number_1.0e+"),
	TESTP("n_number_1.0e-"),
	TESTP("n_number_1.0e"),
	TESTP("n_number_-1.0."),
	TESTP("n_number_1eE2"),
	TESTP("n_number_+1"),
	TESTP("n_number_.-1"),
	TESTP("n_number_2.e+3"),
	TESTP("n_number_2.e-3"),
	TESTP("n_number_2.e3"),
	TESTP("n_number_.2e-3"),
	TESTP("n_number_-2."),
	TESTP("n_number_9.e+"),
	TESTP("n_number_expression"),
	TESTP("n_number_hex_1_digit"),
	TESTP("n_number_hex_2_digits"),
	TESTP("n_number_infinity"),
	TESTP("n_number_+Inf"),
	TESTP("n_number_Inf"),
	TESTP("n_number_invalid+-"),
	TESTP("n_number_invalid-negative-real"),
	TESTP("n_number_invalid-utf-8-in-bigger-int"),
	TESTP("n_number_invalid-utf-8-in-exponent"),
	TESTP("n_number_invalid-utf-8-in-int"),
	TESTP("n_number_++"),
	TESTP("n_number_minus_infinity"),
	TESTP("n_number_minus_sign_with_trailing_garbage"),
	TESTP("n_number_minus_space_1"),
	TESTP("n_number_-NaN"),
	TESTP("n_number_NaN"),
	TESTP("n_number_neg_int_starting_with_zero"),
	TESTP("n_number_neg_real_without_int_part"),
	TESTP("n_number_neg_with_garbage_at_end"),
	TESTP("n_number_real_garbage_after_e"),
	TESTP("n_number_real_with_invalid_utf8_after_e"),
	TESTP("n_number_real_without_fractional_part"),
	TESTP("n_number_starting_with_dot"),
	TESTP("n_number_U+FF11_fullwidth_digit_one"),
	TESTP("n_number_with_alpha_char"),
	TESTP("n_number_with_alpha"),
	TESTP("n_number_with_leading_zero"),
	TESTP("n_object_bad_value"),
	TESTP("n_object_bracket_key"),
	TESTP("n_object_comma_instead_of_colon"),
	TESTP("n_object_double_colon"),
	TESTP("n_object_emoji"),
	TESTP("n_object_garbage_at_end"),
	TESTP("n_object_key_with_single_quotes"),
	TESTP("n_object_lone_continuation_byte_in_key_and_trailing_comma"),
	TESTP("n_object_missing_colon"),
	TESTP("n_object_missing_key"),
	TESTP("n_object_missing_semicolon"),
	TESTP("n_object_missing_value"),
	TESTP("n_object_no-colon"),
	TESTP("n_object_non_string_key_but_huge_number_instead"),
	TESTP("n_object_non_string_key"),
	TESTP("n_object_repeated_null_null"),
	TESTP("n_object_several_trailing_commas"),
	TESTP("n_object_single_quote"),
	TESTP("n_object_trailing_comma"),
	TESTP("n_object_trailing_comment"),
	TESTP("n_object_trailing_comment_open"),
	TESTP("n_object_trailing_comment_slash_open_incomplete"),
	TESTP("n_object_trailing_comment_slash_open"),
	TESTP("n_object_two_commas_in_a_row"),
	TESTP("n_object_unquoted_key"),
	TESTP("n_object_unterminated-value"),
	TESTP("n_object_with_single_string"),
	TESTP("n_object_with_trailing_garbage"),
	TESTP("n_single_space"),
	TESTP("n_string_1_surrogate_then_escape"),
	TESTP("n_string_1_surrogate_then_escape_u1"),
	TESTP("n_string_1_surrogate_then_escape_u1x"),
	TESTP("n_string_1_surrogate_then_escape_u"),
	TESTP("n_string_accentuated_char_no_quotes"),
	TESTP("n_string_backslash_00"),
	TESTP("n_string_escaped_backslash_bad"),
	TESTP("n_string_escaped_ctrl_char_tab"),
	TESTP("n_string_escaped_emoji"),
	TESTP("n_string_escape_x"),
	TESTP("n_string_incomplete_escaped_character"),
	TESTP("n_string_incomplete_escape"),
	TESTP("n_string_incomplete_surrogate_escape_invalid"),
	TESTP("n_string_incomplete_surrogate"),
	TESTP("n_string_invalid_backslash_esc"),
	TESTP("n_string_invalid_unicode_escape"),
	TESTP("n_string_invalid_utf8_after_escape"),
	TESTP("n_string_invalid-utf-8-in-escape"),
	TESTP("n_string_leading_uescaped_thinspace"),
	TESTP("n_string_no_quotes_with_bad_escape"),
	TESTP("n_string_single_doublequote"),
	TESTP("n_string_single_quote"),
	TESTP("n_string_single_string_no_double_quotes"),
	TESTP("n_string_start_escape_unclosed"),
	TESTP("n_string_unescaped_ctrl_char"),
	TESTP("n_string_unescaped_newline"),
	TESTP("n_string_unescaped_tab"),
	TESTP("n_string_unicode_CapitalU"),
	TESTP("n_string_with_trailing_garbage"),
	TESTP("n_structure_100000_opening_arrays"),
	TESTP("n_structure_angle_bracket_."),
	TESTP("n_structure_angle_bracket_null"),
	TESTP("n_structure_array_trailing_garbage"),
	TESTP("n_structure_array_with_extra_array_close"),
	TESTP("n_structure_array_with_unclosed_string"),
	TESTP("n_structure_ascii-unicode-identifier"),
	TESTP("n_structure_capitalized_True"),
	TESTP("n_structure_close_unopened_array"),
	TESTP("n_structure_comma_instead_of_closing_brace"),
	TESTP("n_structure_double_array"),
	TESTP("n_structure_end_array"),
	TESTP("n_structure_incomplete_UTF8_BOM"),
	TESTP("n_structure_lone-invalid-utf-8"),
	TESTP("n_structure_lone-open-bracket"),
	TESTP("n_structure_no_data"),
	TESTP("n_structure_null-byte-outside-string"),
	TESTP("n_structure_number_with_trailing_garbage"),
	TESTP("n_structure_object_followed_by_closing_object"),
	TESTP("n_structure_object_unclosed_no_value"),
	TESTP("n_structure_object_with_comment"),
	TESTP("n_structure_object_with_trailing_garbage"),
	TESTP("n_structure_open_array_apostrophe"),
	TESTP("n_structure_open_array_comma"),
	TESTP("n_structure_open_array_object"),
	TESTP("n_structure_open_array_open_object"),
	TESTP("n_structure_open_array_open_string"),
	TESTP("n_structure_open_array_string"),
	TESTP("n_structure_open_object_close_array"),
	TESTP("n_structure_open_object_comma"),
	TESTP("n_structure_open_object"),
	TESTP("n_structure_open_object_open_array"),
	TESTP("n_structure_open_object_open_string"),
	TESTP("n_structure_open_object_string_with_apostrophes"),
	TESTP("n_structure_open_open"),
	TESTP("n_structure_single_eacute"),
	TESTP("n_structure_single_star"),
	TESTP("n_structure_trailing_#"),
	TESTP("n_structure_U+2060_word_joined"),
	TESTP("n_structure_uescaped_LF_before_string"),
	TESTP("n_structure_unclosed_array"),
	TESTP("n_structure_unclosed_array_partial_null"),
	TESTP("n_structure_unclosed_array_unfinished_false"),
	TESTP("n_structure_unclosed_array_unfinished_true"),
	TESTP("n_structure_unclosed_object"),
	TESTP("n_structure_unicode-identifier"),
	TESTP("n_structure_UTF8_BOM_no_data"),
	TESTP("n_structure_whitespace_formfeed"),
	TESTP("n_structure_whitespace_U+2060_word_joiner")
};

Test y_tests[] = {
	TESTP("y_array_arraysWithSpaces"),
	TESTP("y_array_empty"),
	TESTP("y_array_empty-string"),
	TESTP("y_array_ending_with_newline"),
	TESTP("y_array_false"),
	TESTP("y_array_heterogeneous"),
	TESTP("y_array_null"),
	TESTP("y_array_with_1_and_newline"),
	TESTP("y_array_with_leading_space"),
	TESTP("y_array_with_several_null"),
	TESTP("y_array_with_trailing_space"),
	TESTP("y_number_0e+1"),
	TESTP("y_number_0e1"),
	TESTP("y_number_after_space"),
	TESTP("y_number_double_close_to_zero"),
	TESTP("y_number_int_with_exp"),
	TESTP("y_number"),
	TESTP("y_number_minus_zero"),
	TESTP("y_number_negative_int"),
	TESTP("y_number_negative_one"),
	TESTP("y_number_negative_zero"),
	TESTP("y_number_real_capital_e"),
	TESTP("y_number_real_capital_e_neg_exp"),
	TESTP("y_number_real_capital_e_pos_exp"),
	TESTP("y_number_real_exponent"),
	TESTP("y_number_real_fraction_exponent"),
	TESTP("y_number_real_neg_exp"),
	TESTP("y_number_real_pos_exponent"),
	TESTP("y_number_simple_int"),
	TESTP("y_number_simple_real"),
	TESTP("y_object_basic"),
	TESTP("y_object_duplicated_key_and_value"),
	TESTP("y_object_duplicated_key"),
	TESTP("y_object_empty"),
	TESTP("y_object_empty_key"),
	TESTP("y_object_escaped_null_in_key"),
	TESTP("y_object_extreme_numbers"),
	TESTP("y_object"),
	TESTP("y_object_long_strings"),
	TESTP("y_object_simple"),
	TESTP("y_object_string_unicode"),
	TESTP("y_object_with_newlines"),
	TESTP("y_string_1_2_3_bytes_UTF-8_sequences"),
	TESTP("y_string_accepted_surrogate_pair"),
	TESTP("y_string_accepted_surrogate_pairs"),
	TESTP("y_string_allowed_escapes"),
	TESTP("y_string_backslash_and_u_escaped_zero"),
	TESTP("y_string_backslash_doublequotes"),
	TESTP("y_string_comments"),
	TESTP("y_string_double_escape_a"),
	TESTP("y_string_double_escape_n"),
	TESTP("y_string_escaped_control_character"),
	TESTP("y_string_escaped_noncharacter"),
	TESTP("y_string_in_array"),
	TESTP("y_string_in_array_with_leading_space"),
	TESTP("y_string_last_surrogates_1_and_2"),
	TESTP("y_string_nbsp_uescaped"),
	TESTP("y_string_nonCharacterInUTF-8_U+10FFFF"),
	TESTP("y_string_nonCharacterInUTF-8_U+FFFF"),
	TESTP("y_string_null_escape"),
	TESTP("y_string_one-byte-utf-8"),
	TESTP("y_string_pi"),
	TESTP("y_string_reservedCharacterInUTF-8_U+1BFFF"),
	TESTP("y_string_simple_ascii"),
	TESTP("y_string_space"),
	TESTP("y_string_surrogates_U+1D11E_MUSICAL_SYMBOL_G_CLEF"),
	TESTP("y_string_three-byte-utf-8"),
	TESTP("y_string_two-byte-utf-8"),
	TESTP("y_string_u+2028_line_sep"),
	TESTP("y_string_u+2029_par_sep"),
	TESTP("y_string_uescaped_newline"),
	TESTP("y_string_uEscape"),
	TESTP("y_string_unescaped_char_delete"),
	TESTP("y_string_unicode_2"),
	TESTP("y_string_unicodeEscapedBackslash"),
	TESTP("y_string_unicode_escaped_double_quote"),
	TESTP("y_string_unicode"),
	TESTP("y_string_unicode_U+10FFFE_nonchar"),
	TESTP("y_string_unicode_U+1FFFE_nonchar"),
	TESTP("y_string_unicode_U+200B_ZERO_WIDTH_SPACE"),
	TESTP("y_string_unicode_U+2064_invisible_plus"),
	TESTP("y_string_unicode_U+FDD0_nonchar"),
	TESTP("y_string_unicode_U+FFFE_nonchar"),
	TESTP("y_string_utf8"),
	TESTP("y_string_with_del_character"),
	TESTP("y_structure_lonely_false"),
	TESTP("y_structure_lonely_int"),
	TESTP("y_structure_lonely_negative_real"),
	TESTP("y_structure_lonely_null"),
	TESTP("y_structure_lonely_string"),
	TESTP("y_structure_lonely_true"),
	TESTP("y_structure_string_empty"),
	TESTP("y_structure_trailing_newline"),
	TESTP("y_structure_true_in_array"),
	TESTP("y_structure_whitespace_array"),
};

const char *ANSI_UNDERLINE = "\033[4m";
const char *ANSI_BOLD = "\033[1m";
const char *ANSI_RESET = "\033[0m";
const char *ANSI_RED = "\033[31m";
const char *ANSI_GREEN = "\033[32m";

static string format_string(string_view sv) {
	string result;
	string_new(&result, "", ga);
	for (size_t i = 0; i < sv.size; i++) {
		uchar c = sv.data[i];
		if (c >= 0x20 && c <= 126) {
			string_append_cstr(&result, (const char[]){(char)c, '\0'}, ga);
		} else {
			string_append_cstr(&result, ANSI_UNDERLINE, ga);
			string_append_cstr(&result, "0x", ga);
			char buf[3];
			snprintf(buf, sizeof(buf), "%02X", c);
			string_append_cstr(&result, buf, ga);
			string_append_cstr(&result, ANSI_RESET, ga);
		}
	}
	return result;
}

static Result format_test_input(string *input, const char *filename) {
	string json;
	Result r = read_file_to_string(filename, &json);
	if (!r.success) return r;

	*input = format_string(as_sv(json));
	string_free(&json, ga);
	return new_success();
}

static void format_test_input_internal(string *input, const char *filename) {
	Result r = format_test_input(input, filename);
	if (!r.success) {
		string err_msg = format_error(r);
		fprintf(stderr, "Error formatting test input from %s: %.*s\n", filename,
				(int)err_msg.arr.length, err_msg.arr.data);
		string_free(&err_msg, ga);
		exit(EXIT_FAILURE);
	}
	error_free(r);
}

static void expect_error(RealResult rr, const char *test_name, string_view context) {
	size_t len = context.size > 100 ? 100 : context.size;
	const char *ellipsis = len < context.size ? "..." : "";

	if (rr.success) {
		printf("Test %s%s%s (%.*s%s): %sFAILED%s\n",
				ANSI_BOLD, test_name, ANSI_RESET, (int)len, context.data, ellipsis, ANSI_RED, ANSI_RESET);
		printf("Expected error, got success.\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Test %s%s%s (%.*s%s): %sSUCCESS%s\n",
				ANSI_BOLD, test_name, ANSI_RESET, (int)len, context.data, ellipsis, ANSI_GREEN, ANSI_RESET);
		string msg = format_real_result(rr);
		printf("Expected error, got %.*s\n", (int)msg.arr.length, (char *)msg.arr.data);
		string_free(&msg, ga);
	}
}

static void expect_success(RealResult rr, const char *test_name, string_view context) {
	size_t len = context.size > 100 ? 100 : context.size;
	const char *ellipsis = len < context.size ? "..." : "";

	if (!rr.success) {
		printf("Test %s%s%s (%.*s%s): %sFAILED%s\n",
				ANSI_BOLD, test_name, ANSI_RESET, (int)len, context.data, ellipsis, ANSI_RED, ANSI_RESET);
		string msg = format_real_result(rr);
		printf("Expected success, got %.*s\n", (int)msg.arr.length, (char *)msg.arr.data);
		string_free(&msg, ga);
		exit(EXIT_FAILURE);
	} else {
		printf("Test %s%s%s (%.*s%s): %sSUCCESS%s\n",
				ANSI_BOLD, test_name, ANSI_RESET, (int)len, context.data, ellipsis, ANSI_GREEN, ANSI_RESET);
		printf("Expected success, got success.\n");
	}
}

static void run_n_test_once(Test t) {
	RealResult rr = cerrno_store(run_once(t.filename));

	string input;
	format_test_input_internal(&input, t.filename);
	expect_error(rr, t.name, as_sv(input));
	string_free(&input, ga);
	real_result_free(rr);

	if (sv_startswith(as_svc(t.name), svl("n_string")) || sv_startswith(as_svc(t.name), svl("n_number"))) {
		string file_content;
		Result r = read_file_to_string(t.filename, &file_content);
		if (!r.success) {
			string err_msg = format_error(r);
			fprintf(stderr, "Error reading file %s: %.*s\n", t.filename,
					(int)err_msg.arr.length, err_msg.arr.data);
			string_free(&err_msg, ga);
			exit(EXIT_FAILURE);
		}
		if (sv_startswith(as_sv(file_content), svl("[")) &&
			sv_endswith(as_sv(file_content), svl("]"))) {
			
			string stripped;
			string_from_view(&stripped, sv_substr_unchecked(as_sv(file_content), 1, file_content.arr.length - 2), ga);

			RealResult rr2 = cerrno_store(run_string(&stripped, NULL));

			string stripped_formatted = format_string(as_sv(stripped));

			expect_error(rr2, t.name, as_sv(stripped_formatted));
			string_free(&stripped_formatted, ga);
			real_result_free(rr2);
			string_free(&stripped, ga);
		}
		string_free(&file_content, ga);
	}
}

static void run_y_test_once(Test t) {
	RealResult rr = cerrno_store(run_once(t.filename));

	string input;
	format_test_input_internal(&input, t.filename);
	expect_success(rr, t.name, as_sv(input));
	string_free(&input, ga);
	real_result_free(rr);

	if (sv_startswith(as_svc(t.name), svl("y_string")) || sv_startswith(as_svc(t.name), svl("y_number"))) {
		string file_content;
		read_file_to_string(t.filename, &file_content);
		if (sv_startswith(as_sv(file_content), svl("[")) &&
			sv_endswith(as_sv(file_content), svl("]"))) {
			
			string stripped;
			string_from_view(&stripped, sv_substr_unchecked(as_sv(file_content), 1, file_content.arr.length - 2), ga);

			RealResult rr2 = cerrno_store(run_string(&stripped, NULL));
			expect_success(rr2, t.name, svl("stripped"));
			real_result_free(rr2);
			string_free(&stripped, ga);
		}
		string_free(&file_content, ga);
	}
}

void run_jsontestsuite(void) {
	size_t n_n_tests = sizeof(n_tests) / sizeof(n_tests[0]);
	size_t n_y_tests = sizeof(y_tests) / sizeof(y_tests[0]);

	printf("Running JSON Test Suite - Negative Tests (%zu tests)\n",
		   n_n_tests);
	for (size_t i = 0; i < n_n_tests; i++) {
		run_n_test_once(n_tests[i]);
	}

	printf("Running JSON Test Suite - Positive Tests (%zu tests)\n",
		   n_y_tests);
	for (size_t i = 0; i < n_y_tests; i++) {
		run_y_test_once(y_tests[i]);
	}
}
