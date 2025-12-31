#pragma once
#include "../../utils/custom_error.h"
#include "../../utils/dstring.h"
#include "unicode_types.h"
#include <stdint.h>

/**
 * @brief Shorthand for checking if a character is in a specified range
 * (inklusive)
 */
static inline bool _between(uchar lower, uchar c, uchar upper) {
	return c >= lower && c <= upper;
}

static inline bool _btwcp(UnicodeCodePoint cp, UnicodeCodePoint lower,
						  UnicodeCodePoint upper) {
	return cp >= lower && cp <= upper;
}

Result _get_next_uchar(const string *str, size_t *index, uchar *out);
bool is_surrogate_half(UnicodeCodePoint cp);
bool is_low_surrogate(UnicodeCodePoint cp);
bool is_high_surrogate(UnicodeCodePoint cp);
UnicodeCodePoint decode_surrogate_pair(UnicodeCodePoint high,
									   UnicodeCodePoint low);

/**
 * @brief Returns the first digit of a decimal number
 */
#define _b_first_bit(dec) ((dec) % 10)

/**
 * @brief Recursive macros to build up the binary value
 *
 * Each macro extracts the first digit, and then calls the next macro with the
 * last digit removed, and appends it left of the first digit (= one left shift)
 */
#define _b_b8(num) (_b_first_bit(num) | (_b_b7(num / 10) << 1))
#define _b_b7(num) (_b_first_bit(num) | (_b_b6(num / 10) << 1))
#define _b_b6(num) (_b_first_bit(num) | (_b_b5(num / 10) << 1))
#define _b_b5(num) (_b_first_bit(num) | (_b_b4(num / 10) << 1))
#define _b_b4(num) (_b_first_bit(num) | (_b_b3(num / 10) << 1))
#define _b_b3(num) (_b_first_bit(num) | (_b_b2(num / 10) << 1))
#define _b_b2(num) (_b_first_bit(num) | (_b_b1(num / 10) << 1))
#define _b_b1(num) (_b_first_bit(num))

/**
 * @brief Returns the binary value of a literal with specified length
 */
#define _b_blength(num, length)                                                \
	((length) == 8	 ? _b_b8(num)                                              \
	 : (length) == 7 ? _b_b7(num)                                              \
	 : (length) == 6 ? _b_b6(num)                                              \
	 : (length) == 5 ? _b_b5(num)                                              \
	 : (length) == 4 ? _b_b4(num)                                              \
	 : (length) == 3 ? _b_b3(num)                                              \
	 : (length) == 2 ? _b_b2(num)                                              \
	 : (length) == 1 ? _b_b1(num)                                              \
					 : 0)

/**
 * @brief Counts the number of digits in a binary literal
 */
#define _b_digit_count(literal) (sizeof(#literal) - 1)
/**
 * @brief Binary literal macro
 */
#define b(num) _b_blength(num, _b_digit_count(num))
