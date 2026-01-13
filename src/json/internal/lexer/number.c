#include "number.h"
#include "../../config.h"
#include "utils.h"
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <errno.h>

enum NumParseState {
	NUM_STATE_START,
	NUM_STATE_AFTER_SIGN,
	NUM_STATE_PARSING_INT,
	NUM_STATE_AFTER_INT,
	NUM_STATE_AFTER_DECIMAL_POINT,
	NUM_STATE_PARSING_FRACTION,
	NUM_STATE_AFTER_FRACTION,
	NUM_STATE_AFTER_EXPONENT_SYMBOL,
	NUM_STATE_AFTER_EXPONENT_SIGN,
	NUM_STATE_PARSING_EXPONENT,
	NUM_STATE_AFTER_EXPONENT
};
typedef enum NumParseState NumParseState;

struct ParsedNumber {
	// The sign of the number: 1 for positive, -1 for negative
	int sign;
	// The integer part of the number
	string_view integer_part;
	// Length of the integer part without trailing zeros
	size_t integer_length;

	// The fractional part of the number
	string_view fractional_part;
	// Length of the fractional part without trailing zeros. 0 if there is no
	// fractional part
	size_t fraction_length;

	bool has_exponent;
	int exponent_sign;
	string_view exponent_part;
};
typedef struct ParsedNumber ParsedNumber;

Result expected_error(const char *position, char c, size_t line, size_t column,
					  const char *expected_history[], size_t expected_history_size) {
	string expected_str;
	string_new(&expected_str, "");
	for (size_t i = 0; i < expected_history_size; i++) {
		if (i > 0) {
			string_append_cstr(&expected_str, ", ");
		}
		string_append_cstr(&expected_str, expected_history[i]);
	}

	Result r = new_errorf(
		"Invalid character '%c'%s: expected one of: %.*s (at line %zu, column %zu)", ELexerSyntaxError,
		c, position, (int)expected_str.arr.length, expected_str.arr.data, line, column);
	string_free(&expected_str);
	return r;
}

static Result num_dfa_next_state(NumParseState current_state, UCP chr,
								 NumParseState *next_state, uchar *chr_ptr,
								 ParsedNumber *parsed_number,
								 bool *was_epsilon_transition, bool *has_next,
								 size_t line, size_t column,
								 const char *expected_history[5], size_t *expected_history_size) {
	switch (current_state) {
	case NUM_STATE_START:
		if (chr == '-') {
			parsed_number->sign = -1;
			*next_state = NUM_STATE_AFTER_SIGN;
		} else {
			parsed_number->sign = 1;
			*next_state = NUM_STATE_AFTER_SIGN;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "'-'";
		}
		break;
	case NUM_STATE_AFTER_SIGN:
		if (chr == '0') {
			parsed_number->integer_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_AFTER_INT;
			*expected_history_size = 0;
		} else if (chr >= '1' && chr <= '9') {
			parsed_number->integer_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_PARSING_INT;
			parsed_number->integer_length = 1;
		} else {
			expected_history[(*expected_history_size)++] = "digit";
			return expected_error(" in number", chr, line, column,
								  expected_history, *expected_history_size);
		}
		break;
	case NUM_STATE_PARSING_INT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->integer_part.size++;
			if (chr != '0') {
				parsed_number->integer_length =
					parsed_number->integer_part.size;
			}
			*next_state = NUM_STATE_PARSING_INT;
		} else {
			*next_state = NUM_STATE_AFTER_INT;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "digit";
		}
		break;
	case NUM_STATE_AFTER_INT:
		if (chr == '.') {
			*next_state = NUM_STATE_AFTER_DECIMAL_POINT;
		} else if (parsed_number->integer_part.size == 1 &&
				   parsed_number->integer_part.data[0] == '0' && chr >= '0' &&
				   chr <= '9') {
			// This is a "leading zero" (the current integer part is 0, and the
			// next character is a digit) If we would not catch this here, the
			// lexer would lex something like 01 as two separate numbers: 0 and
			// 1 Which then would be rejected by the parser, as two numbers
			// can't follow each other. In order to provide better error
			// messages, we catch this case here
			return new_errorf("Invalid character '%c' in number: leading zeros "
							  "are not allowed at line %zu, column %zu",
							  ELexerSyntaxError, chr, line, column);
		} else {
			*next_state = NUM_STATE_AFTER_FRACTION;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "'.'";
		}
		break;
	case NUM_STATE_AFTER_DECIMAL_POINT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->fractional_part =
				(string_view){.data = chr_ptr, .size = 1};
			if (chr != '0') {
				parsed_number->fraction_length = 1;
			} else {
				parsed_number->fraction_length = 0;
			}
			*next_state = NUM_STATE_PARSING_FRACTION;
		} else {
			// return new_errorf(
			// 	"Invalid character '%c' in number: expected digit after "
			// 	"decimal point at line %zu, column %zu",
			// 	ELexerSyntaxError, chr, line, column);
			expected_history[(*expected_history_size)++] = "digit";
			return expected_error(" in number after decimal point", chr, line, column,
								  expected_history, *expected_history_size);
		}
		break;
	case NUM_STATE_PARSING_FRACTION:
		if (chr >= '0' && chr <= '9') {
			parsed_number->fractional_part.size++;
			*next_state = NUM_STATE_PARSING_FRACTION;
			if (chr != '0') {
				parsed_number->fraction_length =
					parsed_number->fractional_part.size;
			}
		} else {
			*next_state = NUM_STATE_AFTER_FRACTION;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "digit";
		}
		break;
	case NUM_STATE_AFTER_FRACTION:
		if (chr == 'e' || chr == 'E') {
			*next_state = NUM_STATE_AFTER_EXPONENT_SYMBOL;
		} else {
			*next_state = NUM_STATE_AFTER_EXPONENT;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "'e' or 'E'";
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
			expected_history[(*expected_history_size)++] = "'+' or '-'";
		}
		break;
	case NUM_STATE_AFTER_EXPONENT_SIGN:
		if (chr >= '0' && chr <= '9') {
			parsed_number->exponent_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_PARSING_EXPONENT;
		} else {
			// return new_errorf(
			// 	"Invalid character '%c' in number: expected digit after "
			// 	"exponent sign at line %zu, column %zu",
			// 	ELexerSyntaxError, chr, line, column);
			expected_history[(*expected_history_size)++] = "digit";
			return expected_error(" in number after exponent sign", chr, line, column,
								  expected_history, *expected_history_size);
		}
		break;
	case NUM_STATE_PARSING_EXPONENT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->exponent_part.size++;
			*next_state = NUM_STATE_PARSING_EXPONENT;
		} else {
			*next_state = NUM_STATE_AFTER_EXPONENT;
			*was_epsilon_transition = true;
			expected_history[(*expected_history_size)++] = "digit";
		}
		break;
	case NUM_STATE_AFTER_EXPONENT:
		*has_next = false;
		break;
	}
	if (!*was_epsilon_transition) {
		*expected_history_size = 0;
	}
	return new_success();
}

enum OverflowError { TOO_LARGE, TOO_SMALL, NO_OVERFLOW };
typedef enum OverflowError OverflowError;

/**
 * @brief Takes two strings and a number of how many digits to look at, and
 * turns it into an integer. If `steps` is larger than the combined length of
 * both strings, the remaining digits are treated as zeros.
 */
static OverflowError join_to_int(size_t steps, string_view first,
								 string_view second, int sign,
								 intmax_t *integer) {
	for (size_t i = 0; i < steps; i++) {
		uchar digit;
		if (i < first.size) {
			digit = first.data[i] - '0';
		} else {
			size_t index_int_second_part = i - first.size;
			if (index_int_second_part < second.size) {
				digit = second.data[index_int_second_part] - '0';
			} else {
				// Pad with zeros
				digit = 0;
				if (*integer == 0) {
					// Integer is zero so no amount of padding will change it
					return NO_OVERFLOW;
				}
			}
		}
		if (sign == 1) {
			if (*integer > (INTMAX_MAX - digit) / 10) {
				// Could not turn into integer without overflow
				return TOO_LARGE;
			}
			*integer = *integer * 10 + digit;
		} else {
			if (*integer < (INTMAX_MIN + digit) / 10) {
				// Could not turn into integer without overflow
				return TOO_SMALL;
			}
			*integer = *integer * 10 - digit;
		}
	}
	return NO_OVERFLOW;
}

static Result convert_exponent(ParserConfig *config,
							   ParsedNumber *parsed_number,
							   intmax_t *exponent_value, Lexer *lexer, bool *double_fallback) {
	*exponent_value = 0;
	if (parsed_number->has_exponent) {
		if (parsed_number->exponent_sign == 1) {
			for (size_t i = 0; i < parsed_number->exponent_part.size; i++) {
				uchar digit = parsed_number->exponent_part.data[i] - '0';
				if (*exponent_value > (INTMAX_MAX - digit) / 10) {
					// Overflow
					switch (config->exponent_overflow_behavior) {
					case CONFIG_EXPONENT_OVERFLOW_CLAMP:
						*exponent_value = INTMAX_MAX;
						break;
					case CONFIG_EXPONENT_OVERFLOW_ERROR:
						return new_errorf("Exponent larger than maximum "
										  "supported value %" PRIdMAX
										  " at line %zu, column %zu",
										  ELexerNumberOverflow, INTMAX_MAX,
										  lexer->line, lexer->column);
					case CONFIG_EXPONENT_OVERFLOW_DOUBLE_FALLBACK:
						*double_fallback = true;
						return new_success();
					}
				}
				*exponent_value = *exponent_value * 10 + digit;
			}
		} else {
			for (size_t i = 0; i < parsed_number->exponent_part.size; i++) {
				uchar digit = parsed_number->exponent_part.data[i] - '0';
				if (*exponent_value < (INTMAX_MIN + digit) / 10) {
					// Overflow
					switch (config->exponent_overflow_behavior) {
					case CONFIG_EXPONENT_OVERFLOW_CLAMP:
						*exponent_value = INTMAX_MIN;
						break;
					case CONFIG_EXPONENT_OVERFLOW_ERROR:
						return new_errorf("Exponent smaller than minimum "
										  "supported value %" PRIdMAX
										  " at line %zu, column %zu",
										  ELexerNumberOverflow, INTMAX_MIN,
										  lexer->line, lexer->column);
					case CONFIG_EXPONENT_OVERFLOW_DOUBLE_FALLBACK:
						*double_fallback = true;
						return new_success();
					}
				}
				*exponent_value = *exponent_value * 10 - digit;
			}
		}
	}
	return new_success();
}

static Result build_json_number(Lexer *lexer, ParserConfig *config,
								ParsedNumber *parsed_number,
								JSONNumber *out_number) {
	intmax_t exponent_value = 0;
	bool double_fallback = false;
	Result r = convert_exponent(config, parsed_number, &exponent_value, lexer, &double_fallback);
	if (!r.success) {
		return r;
	}

	if (!double_fallback) {
		// Needed left shift in order for the number to be able to be represented as
		// integer (ie. no fractional part). Can be negative (meaning we can right
		// shift that many times while keeping integer representation)
		size_t needed_shift = parsed_number->fraction_length;
		if (needed_shift == 0) {
			// There is no fractional part. We have extra leeway depending on how
			// many zeros are at the end of the integer part
			needed_shift =
				-(parsed_number->integer_part.size - parsed_number->integer_length);
		}
		bool can_be_integer = (exponent_value >= (intmax_t)needed_shift);
		if (can_be_integer) {
			intmax_t integer = 0;
			size_t steps = parsed_number->integer_part.size + exponent_value;
			OverflowError integer_overflow_error = join_to_int(
				steps, parsed_number->integer_part, parsed_number->fractional_part,
				parsed_number->sign, &integer);
			if (integer_overflow_error == NO_OVERFLOW) {
				// Successfully turned into integer
				out_number->is_integer = true;
				out_number->int_value = integer;
				return new_success();
			} else {
				// Could not turn into integer without overflow
				switch (config->integer_overflow_behavior) {
				case CONFIG_INTEGER_OVERFLOW_CLAMP:
					if (integer_overflow_error == TOO_LARGE) {
						out_number->is_integer = true;
						out_number->int_value = INTMAX_MAX;
					} else {
						out_number->is_integer = true;
						out_number->int_value = INTMAX_MIN;
					}
					return new_success();
				case CONFIG_INTEGER_OVERFLOW_DOUBLE_FALLBACK:
					// Fall back to double representation
					break;
				case CONFIG_INTEGER_OVERFLOW_ERROR:
					if (integer_overflow_error == TOO_LARGE) {
						return new_errorf("Integer value larger than maximum "
										"supported value %" PRIdMAX
										" at line %zu, column %zu",
										ELexerNumberOverflow, INTMAX_MAX,
										lexer->line, lexer->column);
					} else {
						return new_errorf("Integer value smaller than minimum "
										"supported value %" PRIdMAX
										" at line %zu, column %zu",
										ELexerNumberOverflow, INTMAX_MIN,
										lexer->line, lexer->column);
					}
				}
			}
		}
	}

	// Build string representation of the number
	// With variable array size
	#ifdef __STDC_NO_VLA__
	#error "Variable Length Arrays are required for this function"
	#endif

	const size_t sign_space = (parsed_number->sign == -1 ? 1 : 0);
	const size_t int_space = parsed_number->integer_part.size;
	const size_t decimal_point_space = (parsed_number->fractional_part.size > 0 ? 1 : 0);
	const size_t frac_space = parsed_number->fractional_part.size;
	const size_t exponent_space = (parsed_number->has_exponent ? 2 : 0);
	const size_t exp_part_space = parsed_number->exponent_part.size;
	const size_t buffer_size = sign_space + int_space + decimal_point_space +
							   frac_space + exponent_space + exp_part_space + 1;
	uchar buffer[buffer_size];
	size_t buffer_index = 0;
	if (parsed_number->sign == -1) {
		buffer[buffer_index++] = '-';
	}
	memcpy(buffer + buffer_index, parsed_number->integer_part.data,
		   parsed_number->integer_part.size);
	buffer_index += parsed_number->integer_part.size;
	if (parsed_number->fractional_part.size > 0) {
		buffer[buffer_index++] = '.';
		memcpy(buffer + buffer_index, parsed_number->fractional_part.data,
			   parsed_number->fractional_part.size);
		buffer_index += parsed_number->fractional_part.size;
	}
	if (parsed_number->has_exponent) {
		buffer[buffer_index++] = 'e';
		if (parsed_number->exponent_sign == -1) {
			buffer[buffer_index++] = '-';
		} else {
			buffer[buffer_index++] = '+';
		}
		memcpy(buffer + buffer_index, parsed_number->exponent_part.data,
			   parsed_number->exponent_part.size);
		buffer_index += parsed_number->exponent_part.size;
	}
	buffer[buffer_index] = '\0';

	char *endptr;
	errno = 0;
	double result = strtod((char*)buffer, &endptr);
    if ((result == HUGE_VAL || result == -HUGE_VAL) && errno == ERANGE) {
        switch (config->double_overflow_behavior) {
        case CONFIG_DOUBLE_OVERFLOW_CLAMP:
            if (result > 0) {
                out_number->is_integer = false;
                out_number->float_value = DBL_MAX;
            } else {
                out_number->is_integer = false;
                out_number->float_value = -DBL_MAX;
            }
            return new_success();
        case CONFIG_DOUBLE_OVERFLOW_INF:
            out_number->is_integer = false;
			if (result > 0) {
				out_number->float_value = INFINITY;
			} else {
				out_number->float_value = -INFINITY;
			}
            return new_success();
        case CONFIG_DOUBLE_OVERFLOW_ERROR:
			return new_errorf("Double value out of range at line %zu, "
							  "column %zu",
							  ELexerNumberOverflow, lexer->line,
							  lexer->column);
		default:
			panic("Unhandled ConfigDoubleOutOfRangeBehavior");
        }
    } else if (endptr != (char*)buffer) {
        out_number->is_integer = false;
        out_number->float_value = result;
        return new_success();
    } else {
		return new_errorf("Could not convert number to double at line %zu, "
						  "column %zu",
						  ELexerSyntaxError, lexer->line,
						  lexer->column);
	}
}

Result lexer_lex_number(Lexer *lexer, JSONToken *token) {
	string value;
	string_new(&value, "");
	UCP chr;
	Result r;

	NumParseState state = NUM_STATE_START;
	ParsedNumber parsed_number = {0};
	parsed_number.has_exponent = false;

	const char *expected_history[5];
	size_t expected_history_size = 0;

	while (1) {
		r = lexer_peek(lexer, &chr);
		if (r.type == ELexerEOF) {
			// Reached end of file
			// Check if we are in an accepting state
			if (state != NUM_STATE_PARSING_INT &&
				state != NUM_STATE_AFTER_INT &&
				state != NUM_STATE_PARSING_FRACTION &&
				state != NUM_STATE_AFTER_FRACTION &&
				state != NUM_STATE_PARSING_EXPONENT &&
				state != NUM_STATE_AFTER_EXPONENT) {
				return new_errorf("Unexpected end of input in number at line %zu, column %zu",
								  ELexerSyntaxError, lexer->line, lexer->column);
			}
			
			token->type = JSONTok_Number;
			token->value = value;
			// Build number
			JSONNumber number;
			Result r = build_json_number(lexer, &lexer->config, &parsed_number, &number);
			if (!r.success) {
				string_free(&value);
				return r;
			}
			token->number = number;
			error_free(r);
			return new_success();
		} else if (!r.success) {
			string_free(&value);
			return r;
		}

		bool was_epsilon_transition = false;
		NumParseState next_state;
		bool has_next = true;

		r = num_dfa_next_state(
			state,		 // Current state of the DFA
			chr,		 // Current character
			&next_state, // Next state of the DFA
			lexer->source->arr.data +
				lexer->position,	 // Pointer to current character
			&parsed_number,			 // Parsed number being built
			&was_epsilon_transition, // Whether the transition is epsilon
			&has_next,				 // Whether there is a next character
			lexer->line,			 // Current line for error reporting
			lexer->column,			 // Current column for error reporting
			expected_history,		 // Array of expected tokens for error reporting
			&expected_history_size	 // Size of the expected tokens array
		);
		if (!r.success) {
			string_free(&value);
			return r;
		}
		if (!has_next) {
			// Reached end of number
			token->type = JSONTok_Number;
			token->value = value;
			// Build number
			JSONNumber number;
			Result r = build_json_number(lexer, &lexer->config, &parsed_number, &number);
			if (!r.success) {
				string_free(&value);
				return r;
			}
			token->number = number;
			return new_success();
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
