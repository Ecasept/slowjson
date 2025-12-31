#pragma once
#include "../../utils/custom_error.h"
#include "../../utils/dstring.h"
#include "unicode_types.h"
#include <stdint.h>

/**
 * @brief Shorthand for checking if a character is in a specified range
 * (inklusive)
 */
static inline bool between(uchar lower, uchar c, uchar upper) {
	return c >= lower && c <= upper;
}

static inline bool btwcp(UnicodeCodePoint lower, UnicodeCodePoint cp,
						  UnicodeCodePoint upper) {
	return cp >= lower && cp <= upper;
}

Result get_next_uchar(const string *str, size_t *index, uchar *out);

/**
 * @brief Returns the first digit of a decimal number
 */
#define b_first_bit(dec) ((dec) % 10)

/**
 * @brief Recursive macros to build up the binary value
 *
 * Each macro extracts the first digit, and then calls the next macro with the
 * last digit removed, and appends it left of the first digit (= one left shift)
 */
#define b_b16(num) (b_first_bit(num) | (b_b15(num / 10) << 1))
#define b_b15(num) (b_first_bit(num) | (b_b14(num / 10) << 1))
#define b_b14(num) (b_first_bit(num) | (b_b13(num / 10) << 1))
#define b_b13(num) (b_first_bit(num) | (b_b12(num / 10) << 1))
#define b_b12(num) (b_first_bit(num) | (b_b11(num / 10) << 1))
#define b_b11(num) (b_first_bit(num) | (b_b10(num / 10) << 1))
#define b_b10(num) (b_first_bit(num) | (b_b9(num / 10) << 1))
#define b_b9(num) (b_first_bit(num) | (b_b8(num / 10) << 1))
#define b_b8(num) (b_first_bit(num) | (b_b7(num / 10) << 1))
#define b_b7(num) (b_first_bit(num) | (b_b6(num / 10) << 1))
#define b_b6(num) (b_first_bit(num) | (b_b5(num / 10) << 1))
#define b_b5(num) (b_first_bit(num) | (b_b4(num / 10) << 1))
#define b_b4(num) (b_first_bit(num) | (b_b3(num / 10) << 1))
#define b_b3(num) (b_first_bit(num) | (b_b2(num / 10) << 1))
#define b_b2(num) (b_first_bit(num) | (b_b1(num / 10) << 1))
#define b_b1(num) (b_first_bit(num))

/**
 * @brief Returns the binary value of a literal with specified length
 */
#define b_blength(num, length)                                                \
	((length) == 16 ? b_b16(num)                                              \
	 : (length) == 15 ? b_b15(num)                                            \
	 : (length) == 14 ? b_b14(num)                                            \
	 : (length) == 13 ? b_b13(num)                                            \
	 : (length) == 12 ? b_b12(num)                                            \
	 : (length) == 11 ? b_b11(num)                                            \
	 : (length) == 10 ? b_b10(num)                                            \
	 : (length) == 9 ? b_b9(num)                                              \
	 : (length) == 8 ? b_b8(num)                                              \
	 : (length) == 7 ? b_b7(num)                                              \
	 : (length) == 6 ? b_b6(num)                                              \
	 : (length) == 5 ? b_b5(num)                                              \
	 : (length) == 4 ? b_b4(num)                                              \
	 : (length) == 3 ? b_b3(num)                                              \
	 : (length) == 2 ? b_b2(num)                                              \
	 : (length) == 1 ? b_b1(num)                                              \
					 : 0)

/**
 * @brief Counts the number of digits in a binary literal
 */
#define b_digit_count(literal) (sizeof(#literal) - 1)
/**
 * @brief Binary literal macro
 */
#define b(num) b_blength(num, b_digit_count(num))
