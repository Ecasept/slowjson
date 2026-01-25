#include "number.h"
#include "../../config.h"
#include "utils.h"
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <errno.h>



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
								size_t start_index,
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

	char *endptr;
	errno = 0;
	// Use the original source buffer with start index
	const char *start_ptr = (const char *)(lexer->decoder.source.data + start_index);
	
	double result = strtod(start_ptr, &endptr);
	
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
    } else {
		// Valid double.
        out_number->is_integer = false;
        out_number->float_value = result;
        return new_success();
    }
}

#define next_or_end do { \
	/* Skip previous peeked uchar */ \
	lexer_skip(lexer, 1); \
	/* Get next uchar */ \
	Result r = lexer_peek_uchar(lexer, &chr); \
	if (!r.success) { \
		error_free(r); \
		goto end_of_number; \
	} \
} while(0)

#define next_or_error do { \
	/* Skip previous peeked uchar */ \
	lexer_skip(lexer, 1); \
	/* Get next uchar */ \
	Result r = lexer_peek_uchar(lexer, &chr); \
	if (!r.success) { \
		if (cerrno.type == ELexerEOF) { \
			error_free(r); \
			return new_errorf("Unexpected end of input in number at " \
								  "line %zu, column %zu", \
								  ELexerSyntaxError, lexer->line, \
								  lexer->column); \
		} else { \
			return r; \
		} \
	} \
} while(0)

Result lexer_lex_number(Lexer *lexer, JSONToken *token, uchar chr) {
	ParsedNumber parsed_number = {0};

	size_t start_index = utf8_decoder_pos(&lexer->decoder);

	// Parse sign
	if (chr == '-') {
		parsed_number.sign = -1;
		next_or_error;
	} else {
		parsed_number.sign = 1;
	}

	// Parse integer part
	if (chr >= '0' && chr <= '9') {
		parsed_number.integer_part.data =
			lexer->decoder.source.data + utf8_decoder_pos(&lexer->decoder);
		parsed_number.integer_part.size = 0;
		
		// Leading zero means that no other digits are allowed in the integer part
		if (chr == '0') {
			parsed_number.integer_part.size = 1;
			next_or_end;

			// If the next character is a digit, this is a leading zero error
			if (chr >= '0' && chr <= '9') {
				return new_errorf("Invalid character '%c' in number: leading zeros "
                                  "are not allowed at line %zu, column %zu",
                                  ELexerSyntaxError, chr, lexer->line, lexer->column);
			}
			parsed_number.integer_length = 0;

		} else {
			// Normal number
			while (chr >= '0' && chr <= '9') {
				parsed_number.integer_part.size++;
				// Update integer_length (strip trailing zeros)
				if (chr != '0') {
					parsed_number.integer_length = parsed_number.integer_part.size;
				}
				next_or_end;
			}
		}
	} else {
		return new_errorf("Invalid character '%c' in number at line %zu, column %zu",
						  ELexerSyntaxError, chr, lexer->line, lexer->column);
	}

	// Parse fractional part
	if (chr == '.') {
		next_or_error;
		parsed_number.fractional_part.data =
			lexer->decoder.source.data + utf8_decoder_pos(&lexer->decoder);
		parsed_number.fractional_part.size = 0;
		if (chr >= '0' && chr <= '9') {
			while (chr >= '0' && chr <= '9') {
				parsed_number.fractional_part.size++;
				if (chr != '0') {
					parsed_number.fraction_length =
						parsed_number.fractional_part.size;
				}
				next_or_end;
			}
		} else {
			return new_errorf("Invalid character '%c' in number: expected digit after "
							  "decimal point at line %zu, column %zu",
							  ELexerSyntaxError, chr, lexer->line, lexer->column);
		}
	}

	// Parse exponent part
	if (chr == 'e' || chr == 'E') {
		next_or_error;
		parsed_number.has_exponent = true;
		
		if (chr == '+' || chr == '-') {
			parsed_number.exponent_sign = (chr == '-') ? -1 : 1;
			next_or_error;
		} else {
			parsed_number.exponent_sign = 1;
		}

		if (chr >= '0' && chr <= '9') {
			parsed_number.exponent_part.data =
				lexer->decoder.source.data + utf8_decoder_pos(&lexer->decoder);
			parsed_number.exponent_part.size = 0;

			while (chr >= '0' && chr <= '9') {
				parsed_number.exponent_part.size++;
				next_or_end;
			}
		} else {
			return new_errorf("Invalid character '%c' in number: expected digit after "
							  "exponent sign at line %zu, column %zu",
							  ELexerSyntaxError, chr, lexer->line, lexer->column);
		}
	}
end_of_number:;
	token->type = JSONTok_Number;
	return build_json_number(lexer, &lexer->config, &parsed_number, start_index, &token->number);
}
