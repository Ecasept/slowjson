
#include "lexer.h"
#include "../../utils/unicode/utf8.h"
#include "../../utils/unicode/utf16.h"
#include <ctype.h>

// ==== Relevant Specifications ====
// JSON Website (with syntax diagram):
// https://www.json.org/json-en.html
// RFC JSON Specification:
// https://datatracker.ietf.org/doc/html/rfc8259
// ECMA JSON Standard:
// https://ecma-international.org/publications-and-standards/standards/ecma-404/
// UTF-8 Website with resources:
// https://www.utf8.com/
// Unicode Standard:
// https://www.unicode.org/versions/Unicode17.0.0/UnicodeStandard-17.0.pdf
// UTF-8 Standard:
// https://www.ietf.org/rfc/rfc3629.txt

// ==== Forward Declarations ====
static Result lexer_peek(const Lexer *lexer, UCP *out);
static Result lexer_consume(Lexer *lexer, UCP *out);

static Result lexer_lex_structural(Lexer *lexer, JSONToken *token);
static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token);
static Result lexer_lex_string(Lexer *lexer, JSONToken *token);
static Result lexer_lex_number(Lexer *lexer, JSONToken *token);
static Result lexer_lex_literal(Lexer *lexer, JSONToken *token);
static Result lexer_test_literal(Lexer *lexer, string *string, bool *res);



const char *JSONTokenTypeStrings[] = {FOREACH_TOKEN(DECLARE_TOKEN_STRING)};

static double pow(double x, double y) {
	if (y == 0) {
		return 1;
	}
	double result = x;
	for (int i = 1; i < (int)y; i++) {
		result *= x;
	}
	return result;
}
void lexer_init(Lexer *lexer, const string *source) {
	lexer->source = source;
	lexer->position = 0;
	lexer->line = 0;
	lexer->column = 0;
}

static Result lexer_peek(const Lexer *lexer, UCP *out) {
	size_t pos_copy =
		lexer->position; // Don't pass the actual position. We are just peeking
	Result r = utf8_get_next_codepoint(lexer->source, &pos_copy, out);
	if (r.type == EUnicodeUnexpectedEndOfString) {
		error_free(r);
		return new_error("Unexpected end of input", ELexerEOF);
	} else if (!r.success) {
		return r;
	}
	return new_success();
}

static Result lexer_consume(Lexer *lexer, UCP *out) {
	Result r = utf8_get_next_codepoint(lexer->source, &lexer->position, out);
	if (r.type == EUnicodeUnexpectedEndOfString) {
		error_free(r);
		return new_error("Unexpected end of input", ELexerEOF);
	} else if (!r.success) {
		return r;
	}
	lexer->column++;
	return new_success();
}

Result lexer_next_token(Lexer *lexer, JSONToken *token) {
	token->column = lexer->column;
	token->line = lexer->line;
	token->type = JSONTok_Unknown;


	UCP current_char;
	Result r = lexer_peek(lexer, &current_char);
	if (r.type == ELexerEOF) {
		token->type = JSONTok_EOF;
		string_new(&token->value, "");
		error_free(r);
		return new_success();
	}
	check(r);

	switch (current_char) {
	case '{':
	case '}':
	case '[':
	case ']':
	case ':':
	case ',':
		return lexer_lex_structural(lexer, token);
	case '\n':
	case '\r':
	case '\t':
	case ' ':
		return lexer_lex_whitespace(lexer, token);
	case '"':
		return lexer_lex_string(lexer, token);
	case '-':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return lexer_lex_number(lexer, token);
	default:
		if (isalpha(current_char)) {
			return lexer_lex_literal(lexer, token);
		} else {
			return new_errorf("Unexpected character \"%c\" at line %zu, column %zu (expected start of new token)",
							  ELexerSyntaxError, current_char, lexer->line,
							  lexer->column);
		}
	}
}

static Result lexer_lex_structural(Lexer *lexer, JSONToken *token) {
	// Implementation for lexing structural characters
	UCP current_char;
	check(lexer_consume(lexer, &current_char));

	switch (current_char) {
	case '{':
		token->type = JSONTok_LBrace;
		break;
	case '}':
		token->type = JSONTok_RBrace;
		break;
	case '[':
		token->type = JSONTok_LBracket;
		break;
	case ']':
		token->type = JSONTok_RBracket;
		break;
	case ':':
		token->type = JSONTok_Colon;
		break;
	case ',':
		token->type = JSONTok_Comma;
		break;
	default:
		return new_errorf("Unexpected character \"%c\" at line %zu, column %zu (expected start of structural token)",
						  ELexerSyntaxError, current_char, lexer->line,
						  lexer->column - 1);
	}

	string_new(&token->value, "");
	string_append_uchar(&token->value, current_char);
	return new_success();
}

static Result lexer_lex_whitespace(Lexer *lexer, JSONToken *token) {
	UCP current_char;

	string whitespace;
	string_new(&whitespace, "");

	while (1) {
		Result r = lexer_peek(lexer, &current_char);
		if (r.type == ELexerEOF) {
			// Reached end of file, return what we have
			token->type = JSONTok_Whitespace;
			token->value = whitespace;
			error_free(r);
			return new_success();
		} else if (!r.success) {
			string_free(&whitespace);
			return r;
		}

		switch (current_char) {
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			check(lexer_consume(lexer, &current_char));
			if (current_char == '\n') {
				lexer->column = 0;
				lexer->line++;
			}

			// Append whitespace character to current whitespace
			string_append_uchar(&whitespace, current_char);
			break;
		default:
			// Found non-whitespace character, which means we finished parsing
			// all of the whitespace
			token->type = JSONTok_Whitespace;
			token->value = whitespace;
			return new_success();
		}
	}
}

static Result lexer_test_literal(Lexer *lexer, string *expect, bool *res) {
	string substr;
	Result r = string_substr(lexer->source, &substr, lexer->position,
							 expect->arr.length);
	if (r.type == ESubstrOutOfRange) {
		*res = false;
		error_free(r);
		return new_success();
	} else if (!r.success) {
		return r;
	}

	string_eq(&substr, expect, res);

	string_free(&substr);

	return new_success();
}

static Result lexer_lex_specific_literal(Lexer *lexer, JSONToken *token,
								  string *literal, bool *matched,
								  JSONTokenType type) {
	bool matches;
	check(lexer_test_literal(lexer, literal, &matches));
	if (matches) {
		token->type = type;
		token->value = *literal;
		lexer->position += literal->arr.length;
		lexer->column += literal->arr.length;
		*matched = true;
		return new_success();
	}
	*matched = false;
	return new_success();
}

static Result lexer_lex_literal(Lexer *lexer, JSONToken *token) {
	string true_str;
	string_new(&true_str, "true");
	bool matches;
	check(lexer_lex_specific_literal(lexer, token, &true_str, &matches,
								   JSONTok_True));
	if (matches) {
		return new_success();
	}
	string_free(&true_str);

	string false_str;
	string_new(&false_str, "false");
	check(lexer_lex_specific_literal(lexer, token, &false_str, &matches,
								   JSONTok_False));
	if (matches) {
		return new_success();
	}
	string_free(&false_str);

	string null_str;
	string_new(&null_str, "null");
	check(lexer_lex_specific_literal(lexer, token, &null_str, &matches,
								   JSONTok_Null));
	if (matches) {
		return new_success();
	}
	string_free(&null_str);

	return new_errorf("Unexpected literal at line %zu, column %zu",
					  ELexerSyntaxError, lexer->line, lexer->column);
}

static Result lexer_lex_4_hex(Lexer *lexer, UCP *out) {
	UCP vals[4];
	for (int i = 0; i < 4; i++) {
		UCP c;
		check(lexer_consume(lexer, &c));
		if (c >= 'A' && c <= 'F') {
			vals[i] = c - 'A' + 10;
		} else if (c >= 'a' && c <= 'f') {
			vals[i] = c - 'a' + 10;
		} else if (c >= '0' && c <= '9') {
			vals[i] = c - '0';
		} else {
			return new_errorf("Invalid hex digit \"%c\" at line %zu, column %zu",
							  ELexerSyntaxError, c, lexer->line,
							  lexer->column - 1);
		}
	}
	*out = (vals[0] << 12) | (vals[1] << 8) | (vals[2] << 4) | vals[3];
	return new_success();
}
static Result lexer_lex_unicode_literal(Lexer *lexer, UCP *out) {
	UCP cp;
	check(lexer_lex_4_hex(lexer, &cp));
	if (is_high_surrogate(cp)) {
		UCP backslash;
		UCP u;
		Result r1 = lexer_consume(lexer, &backslash);
		Result r2 = lexer_consume(lexer, &u);
		if (r1.type == ELexerEOF || r2.type == ELexerEOF) {
			return new_error(
				"Unexpected end of file after high surrogate in unicode escape",
				ELexerSyntaxError);
		} else if (!r1.success) {
			return r1;
		} else if (!r2.success) {
			return r2;
		}
		if (backslash != '\\' || u != 'u') {
			return new_errorf("Expected \\u after high surrogate, got \"%c%c\" at line %zu, column %zu",
							  ELexerSyntaxError, backslash, u, lexer->line,
							  lexer->column-2);
		}
		UCP low_surrogate;
		check(lexer_lex_4_hex(lexer, &low_surrogate));
		if (!is_low_surrogate(low_surrogate)) {
			return new_errorf(
				"Expected low surrogate after high surrogate, got U+%04X at line %zu, column %zu",
				ELexerSyntaxError, low_surrogate, lexer->line, lexer->column-2);
		}
		cp = decode_surrogate_pair(cp, low_surrogate);
	} else if (is_low_surrogate(cp)) {
		return new_errorf(
			"Unexpected low surrogate U+%04X without preceding high surrogate at line %zu, column %zu",
			ELexerSyntaxError, cp, lexer->line, lexer->column - 6);
	}
	*out = cp;
	return new_success();
}

static Result lexer_lex_string(Lexer *lexer, JSONToken *token) {
	UCP chr;
	check(lexer_consume(lexer, &chr));
	if (chr != '"') {
		panicf("String called but character was %c", chr);
	}

	string value;
	string_new(&value, "");

	bool is_escaped = false;

	while (1) {
		Result r = lexer_consume(lexer, &chr);
		if (!r.success) {
			string_free(&value);
			return r;
		}

		if (chr <= 0x1F) {
			string_free(&value);
			return new_errorf(
				"Encountered control character with value %u in string at line %zu, column %zu",
				ELexerSyntaxError, chr, lexer->line, lexer->column - 1);
		}

		if (is_escaped) {
			switch (chr) {
			case '"':
				string_append_uchar(&value, '"');
				break;
			case '\\':
				string_append_uchar(&value, '\\');
				break;
			case '/':
				string_append_uchar(&value, '/');
				break;
			case 'b':
				// backspace
				string_append_uchar(&value, '\b');
				break;
			case 'f':
				// form feed
				string_append_uchar(&value, '\f');
				break;
			case 'n':
				// line feed
				string_append_uchar(&value, '\n');
				break;
			case 'r':
				// carriage return
				string_append_uchar(&value, '\r');
				break;
			case 't':
				// tab
				string_append_uchar(&value, '\t');
				break;
			case 'u':
				// unicode escape sequence
				r = lexer_lex_unicode_literal(lexer, &chr);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				r = utf8_append_encoded_codepoint(chr, &value);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				break;
			default:
				string_free(&value);
				return new_errorf("Invalid escape character \\%c at line %zu, column %zu",
								  ELexerSyntaxError, chr, lexer->line, lexer->column - 2);
			}
			is_escaped = false;
		} else {
			switch (chr) {
			case '\\':
				is_escaped = true;
				break;
			case '"':
				token->type = JSONTok_String;
				token->value = value;
				return new_success();
			default:
				r = utf8_append_encoded_codepoint(chr, &value);
				if (!r.success) {
					string_free(&value);
					return r;
				}
				break;
			}
		}
	}
}

enum NumParseState {
	NUM_STATE_ERROR,
	NUM_STATE_ERROR_LEADING_ZERO,
	NUM_STATE_START,
	NUM_STATE_AFTER_SIGN,
	NUM_STATE_AFTER_ONE_INT_DIGIT,
	NUM_STATE_AFTER_INT,
	NUM_STATE_AFTER_DECIMAL_POINT,
	NUM_STATE_AFTER_ONE_FRACTION_DIGIT,
	NUM_STATE_AFTER_FRACTION,
	NUM_STATE_AFTER_EXPONENT_SYMBOL,
	NUM_STATE_AFTER_EXPONENT_SIGN,
	NUM_STATE_AFTER_ONE_EXPONENT_DIGIT,
	NUM_STATE_AFTER_EXPONENT
};

struct ParsedNumber {
	int sign;
	uint64_t integer_part;
	bool has_fraction;
	uint64_t fractional_part;
	uint64_t fraction_length;
	bool has_exponent;
	int exponent_sign;
	uint64_t exponent_part;
};

static bool num_dfa_next_state(enum NumParseState current_state, UCP chr,
						 enum NumParseState *next_state,
						 struct ParsedNumber *parsed_number,
						 bool *was_epsilon_transition) {
	switch (current_state) {
	case NUM_STATE_START:
		if (chr == '-') {
			parsed_number->sign = -1;
			*next_state = NUM_STATE_AFTER_SIGN;
		} else {
			parsed_number->sign = 1;
			*next_state = NUM_STATE_AFTER_SIGN;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_SIGN:
		if (chr == '0') {
			parsed_number->integer_part = 0;
			*next_state = NUM_STATE_AFTER_INT;
		} else if (chr >= '1' && chr <= '9') {
			parsed_number->integer_part = chr - '0';
			*next_state = NUM_STATE_AFTER_ONE_INT_DIGIT;
		} else {
			*next_state = NUM_STATE_ERROR;
		}
		break;
	case NUM_STATE_AFTER_ONE_INT_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->integer_part =
				parsed_number->integer_part * 10 + (chr - '0');
			*next_state = NUM_STATE_AFTER_ONE_INT_DIGIT;
		} else {
			*next_state = NUM_STATE_AFTER_INT;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_INT:
		if (chr == '.') {
			*next_state = NUM_STATE_AFTER_DECIMAL_POINT;
		} else if (parsed_number->integer_part == 0 && chr >= '0' && chr <= '9') {
			// This is a "leading zero"
			// If we woud not catch this here, the lexer would lex something like
			// 01 as two separate numbers: 0 and 1
			// Which then would be rejected by the parser, as two numbers can't follow each other.
			// In order to provide better error messages, we catch this case here
			*next_state = NUM_STATE_ERROR_LEADING_ZERO;
		} else {
			*next_state = NUM_STATE_AFTER_FRACTION;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_DECIMAL_POINT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->has_fraction = true;
			parsed_number->fractional_part = chr - '0';
			parsed_number->fraction_length = 1;
			*next_state = NUM_STATE_AFTER_ONE_FRACTION_DIGIT;
		} else {
			*next_state = NUM_STATE_ERROR;
		}
		break;
	case NUM_STATE_AFTER_ONE_FRACTION_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->fractional_part =
				parsed_number->fractional_part * 10 + (chr - '0');
			parsed_number->fraction_length += 1;
			*next_state = NUM_STATE_AFTER_ONE_FRACTION_DIGIT;
		} else {
			*next_state = NUM_STATE_AFTER_FRACTION;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_FRACTION:
		if (chr == 'e' || chr == 'E') {
			*next_state = NUM_STATE_AFTER_EXPONENT_SYMBOL;
		} else {
			*next_state = NUM_STATE_AFTER_EXPONENT;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_EXPONENT_SYMBOL:
		if (chr == '+' || chr == '-') {
			parsed_number->has_exponent = true;
			*next_state = NUM_STATE_AFTER_EXPONENT_SIGN;
			if (chr == '-') {
				parsed_number->exponent_sign = -1;
			} else {
				parsed_number->exponent_sign = 1;
			}
		} else {
			parsed_number->has_exponent = true;
			parsed_number->exponent_sign = 1;
			*next_state = NUM_STATE_AFTER_EXPONENT_SIGN;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_EXPONENT_SIGN:
		if (chr >= '0' && chr <= '9') {
			parsed_number->exponent_part = chr - '0';
			*next_state = NUM_STATE_AFTER_ONE_EXPONENT_DIGIT;
		} else {
			*next_state = NUM_STATE_ERROR;
		}
		break;
	case NUM_STATE_AFTER_ONE_EXPONENT_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->exponent_part =
				parsed_number->exponent_part * 10 + (chr - '0');
			*next_state = NUM_STATE_AFTER_ONE_EXPONENT_DIGIT;
		} else {
			*next_state = NUM_STATE_AFTER_EXPONENT;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_EXPONENT:
		// End reached
		return false;
	default:
		panicf("Invalid NumParseState: %u", current_state);
	}
	return true;
}

static JSONNumber build_json_number(struct ParsedNumber *parsed_number) {
	JSONNumber number;
	if (!parsed_number->has_fraction && !parsed_number->has_exponent) {
		// Integer
		number.is_integer = true;
		number.int_value = (int64_t)(parsed_number->sign *
									 (int64_t)parsed_number->integer_part);
	} else {
		// Floating point
		number.is_integer = false;
		double value = (double)parsed_number->integer_part;
		if (parsed_number->has_fraction) {
			double fraction = (double)parsed_number->fractional_part;
			value += fraction / pow(10.0, parsed_number->fraction_length);
		}
		if (parsed_number->has_exponent) {
			double exponent_value = pow(10.0, parsed_number->exponent_sign *
												  parsed_number->exponent_part);
			value *= exponent_value;
		}
		value *= parsed_number->sign;
		number.float_value = value;
	}
	return number;
}

static Result lexer_lex_number(Lexer *lexer, JSONToken *token) {
	string value;
	string_new(&value, "");
	UCP chr;
	Result r;

	enum NumParseState state = NUM_STATE_START;
	struct ParsedNumber parsed_number = {0};
	parsed_number.has_fraction = false;
	parsed_number.has_exponent = false;

	while (1) {
		r = lexer_peek(lexer, &chr);
		if (r.type == ELexerEOF) {
			// Reached end of file, return what we have
			token->type = JSONTok_Number;
			token->value = value;
			// Build number
			JSONNumber number = build_json_number(&parsed_number);
			token->number = number;
			error_free(r);
			return new_success();
		} else if (!r.success) {
			string_free(&value);
			return r;
		}

		bool was_epsilon_transition = false;
		enum NumParseState next_state;
		bool has_next = num_dfa_next_state(
			state, chr, &next_state, &parsed_number, &was_epsilon_transition);
		if (!has_next) {
			// Reached end of number
			token->type = JSONTok_Number;
			token->value = value;
			// Build number
			JSONNumber number = build_json_number(&parsed_number);
			token->number = number;
			return new_success();
		} else if (next_state == NUM_STATE_ERROR) {
			string_free(&value);
			return new_errorf("Invalid character \"%c\" in number at line %zu, column %zu",
							  ELexerSyntaxError, chr, lexer->line, lexer->column);
		} else if (next_state == NUM_STATE_ERROR_LEADING_ZERO) {
			string_free(&value);
			return new_errorf("Leading zeros are not allowed in numbers (found at line %zu, column %zu)",
							  ELexerSyntaxError, lexer->line, lexer->column - 1);
		} else {
			if (!was_epsilon_transition) {
				check(lexer_consume(lexer, &chr));
				string_append_uchar(&value, chr);
			}
			state = next_state;
			was_epsilon_transition = false;
		}
	}
}

void lexer_free_token(JSONToken *token) { string_free(&token->value); }

const char *tk_as_str(JSONTokenType type) {
	if (type < 0 || type >= sizeof(JSONTokenTypeStrings) / sizeof(char *)) {
		panicf("Invalid JSONTokenType: %u", type);
	}
	return (const char *)JSONTokenTypeStrings[type];
}
