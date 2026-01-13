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

static Result num_dfa_next_state(NumParseState current_state, UCP chr,
								 NumParseState *next_state, uchar *chr_ptr,
								 ParsedNumber *parsed_number,
								 bool *was_epsilon_transition, bool *has_next,
								 size_t line, size_t column) {
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
			parsed_number->integer_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_AFTER_INT;
		} else if (chr >= '1' && chr <= '9') {
			parsed_number->integer_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_AFTER_ONE_INT_DIGIT;
			parsed_number->integer_length = 1;
		} else {
			return new_errorf("Invalid character '%c' in number: expected "
							  "digit or '-' at line %zu, column %zu",
							  ELexerSyntaxError, chr, line, column);
		}
		break;
	case NUM_STATE_AFTER_ONE_INT_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->integer_part.size++;
			if (chr != '0') {
				parsed_number->integer_length =
					parsed_number->integer_part.size;
			}
			*next_state = NUM_STATE_AFTER_ONE_INT_DIGIT;
		} else {
			*next_state = NUM_STATE_AFTER_INT;
			*was_epsilon_transition = true;
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
			*next_state = NUM_STATE_AFTER_ONE_FRACTION_DIGIT;
		} else {
			return new_errorf(
				"Invalid character '%c' in number: expected digit after "
				"decimal point at line %zu, column %zu",
				ELexerSyntaxError, chr, line, column);
		}
		break;
	case NUM_STATE_AFTER_ONE_FRACTION_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->fractional_part.size++;
			*next_state = NUM_STATE_AFTER_ONE_FRACTION_DIGIT;
			if (chr != '0') {
				parsed_number->fraction_length =
					parsed_number->fractional_part.size;
			}
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
			parsed_number->exponent_part =
				(string_view){.data = chr_ptr, .size = 1};
			*next_state = NUM_STATE_AFTER_ONE_EXPONENT_DIGIT;
		} else {
			return new_errorf(
				"Invalid character '%c' in number: expected digit after "
				"exponent sign at line %zu, column %zu",
				ELexerSyntaxError, chr, line, column);
		}
		break;
	case NUM_STATE_AFTER_ONE_EXPONENT_DIGIT:
		if (chr >= '0' && chr <= '9') {
			parsed_number->exponent_part.size++;
			*next_state = NUM_STATE_AFTER_ONE_EXPONENT_DIGIT;
		} else {
			*next_state = NUM_STATE_AFTER_EXPONENT;
			*was_epsilon_transition = true;
		}
		break;
	case NUM_STATE_AFTER_EXPONENT:
		*has_next = false;
		break;
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
							   intmax_t *exponent_value, Lexer *lexer) {
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
					}
				}
				*exponent_value = *exponent_value * 10 - digit;
			}
		}
	}
	return new_success();
}

#define POWER_OF_2(n) (1ULL << (n))
static const double POWER_OF_10_TABLE[] = {
	1e0,  1e1,  1e2,  1e3,  1e4,  1e5,
	1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
	1e12, 1e13, 1e14, 1e15, 1e16, 1e17,
	1e18, 1e19, 1e20, 1e21, 1e22
};

#define bit_count(literal) (sizeof(#literal) - 1)
#define max(a, b) ((a) > (b) ? (a) : (b))

// + 1 for the sign
static const size_t MAXINT_MAX_DIGIT_COUNT = max(bit_count(INTMAX_MAX), bit_count(INTMAX_MIN) + 1);

static Result build_json_number(Lexer *lexer, ParserConfig *config,
								ParsedNumber *parsed_number,
								JSONNumber *out_number) {
	intmax_t exponent_value = 0;
	Result r = convert_exponent(config, parsed_number, &exponent_value, lexer);
	if (!r.success) {
		return r;
	}

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

	// Convert to double
	// First step: normalize into significant and exponent
	// Example: 123.456e2 -> 123456e-1
	// Example: 12000.0000e2 -> 12e5
	exponent_value -= needed_shift;
	size_t significant_length = parsed_number->integer_part.size + needed_shift;
	intmax_t significant = 0;
	OverflowError significant_overflow_error = join_to_int(
		significant_length, parsed_number->integer_part,
		parsed_number->fractional_part, parsed_number->sign, &significant);
	if (significant_overflow_error != NO_OVERFLOW) {
		switch (config->double_overflow_behavior) {
		case CONFIG_DOUBLE_OVERFLOW_CLAMP:
			if (significant_overflow_error == TOO_LARGE) {
				out_number->is_integer = false;
				out_number->float_value = DBL_MAX;
			} else {
				out_number->is_integer = false;
				out_number->float_value = -DBL_MAX;
			}
			return new_success();
		case CONFIG_DOUBLE_OVERFLOW_INF:
			if (significant_overflow_error == TOO_LARGE) {
				out_number->is_integer = false;
				out_number->float_value = INFINITY;
			} else {
				out_number->is_integer = false;
				out_number->float_value = -INFINITY;
			}
			return new_success();
		case CONFIG_DOUBLE_OVERFLOW_ERROR:
			if (significant_overflow_error == TOO_LARGE) {
				return new_errorf("Significant larger than maximum "
								  "supported value %" PRIdMAX " at line %zu, column %zu",
								  ELexerNumberOverflow, INTMAX_MAX,
								  lexer->line, lexer->column);
			} else {
				return new_errorf("Significant smaller than minimum "
								  "supported value %" PRIdMAX " at line %zu, column %zu",
								  ELexerNumberOverflow, INTMAX_MIN,
								  lexer->line, lexer->column);
			}
			return new_success();
		}
	}

	// Try Clinger's fast path
	bool can_represent_exponent_exact =
		(exponent_value >= -22 && exponent_value <= 22);
	bool can_represent_significant_exact =
		(significant >= -(intmax_t)POWER_OF_2(53) && significant <= (intmax_t)POWER_OF_2(53));
	if (can_represent_exponent_exact && can_represent_significant_exact) {
		double result;
		if (exponent_value < 0) {
			result = (double)significant /
					 POWER_OF_10_TABLE[-exponent_value];
		} else {
			result = (double)significant * POWER_OF_10_TABLE[exponent_value];
		}
		out_number->is_integer = false;
		out_number->float_value = result;
		return new_success();
	}

	// Use strtod as fallback (I'm not implementing the Eisel-Lemire algorithm myself)

	// The significant, "e", exponent, and null terminator
	char buffer[MAXINT_MAX_DIGIT_COUNT + 1 + MAXINT_MAX_DIGIT_COUNT + 1];
	snprintf(buffer, sizeof(buffer), "%" PRIdMAX "e%" PRIdMAX, significant,
			 exponent_value);
	char *endptr;
	errno = 0;
	double result = strtod(buffer, &endptr);
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
    } else if (endptr != buffer) {
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

	while (1) {
		r = lexer_peek(lexer, &chr);
		if (r.type == ELexerEOF) {
			// Reached end of file, return what we have
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
			lexer->column			 // Current column for error reporting
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
