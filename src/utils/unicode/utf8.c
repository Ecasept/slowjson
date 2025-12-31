#include "utils.h"
#include <stdlib.h>
#include <wchar.h>
#include "utf8.h"

// ==== Important Data ===
// Encoding:
// https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/#G27288
// Well-Formed Byte Sequences:
// https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/#G27506

/**
 * @brief Ensures the passed continuation bytes is in its valid range and
 * returns an appropriate error if not
 */
static Result ensure(uchar lower, uchar c, uchar upper) {
	if (!between(lower, c, upper)) {
		return new_errorf(
			"Invalid continuation byte 0x%2X. Expected between 0x%2X and 0x%2X",
			EUnicodeError, c, lower, upper);
	}
	return new_success();
}

/**
 * @brief Ensure the unicode byte is in the default valid range for most
 * continuation bytes
 */
static Result ensure_default(uchar c) { return ensure(0x80, c, 0xBF); }

/**
 * @brief Removes the 0b10 marking at the beginning of every continuation byte
 */
static inline uchar elim(uchar c) { return (b(10000000)) ^ c; }

/**
 * @brief Decodes a single UTF-8 byte into a Unicode codepoint
 */
static void decode_single_byte(uchar c1, UnicodeCodePoint *out) { *out = c1; }
/**
 * @brief Decodes a two-byte UTF-8 sequence into a Unicode codepoint
 */
static void decode_double_byte(uchar c1, uchar c2, UnicodeCodePoint *out) {
	UnicodeCodePoint c1_contrib = ((b(11000000)) ^ c1) << 6;
	UnicodeCodePoint c2_contrib = elim(c2);
	*out = c1_contrib + c2_contrib;
}
/**
 * @brief Decodes a three-byte UTF-8 sequence into a Unicode codepoint
 */
static void decode_triple_byte(uchar c1, uchar c2, uchar c3, UnicodeCodePoint *out) {
	UnicodeCodePoint c1_contrib = ((b(11100000)) ^ c1) << 12;
	UnicodeCodePoint c2_contrib = elim(c2) << 6;
	UnicodeCodePoint c3_contrib = elim(c3);
	*out = c1_contrib + c2_contrib + c3_contrib;
}
/**
 * @brief Decodes a four-byte UTF-8 sequence into a Unicode codepoint
 */
static void decode_quadruple_byte(uchar c1, uchar c2, uchar c3, uchar c4,
							UnicodeCodePoint *out) {
	UnicodeCodePoint c1_contrib = ((b(11110000)) ^ c1) << 18;
	UnicodeCodePoint c2_contrib = elim(c2) << 12;
	UnicodeCodePoint c3_contrib = elim(c3) << 6;
	UnicodeCodePoint c4_contrib = elim(c4);
	*out = c1_contrib + c2_contrib + c3_contrib + c4_contrib;
}

/**
 * @brief Reads the next UTF-8 encoded codepoint from a string
 * @param str The string to read from
 * @param index Pointer to the current index in the string. Will be updated to
 * the next codepoint's index after reading.
 * @param out Pointer to store the resulting codepoint
 */
Result utf8_get_next_codepoint(const string *str, size_t *index,
							   UnicodeCodePoint *out) {
	size_t index_cpy = *index;
	uchar c1;
	Result r;
	check(get_next_uchar(str, &index_cpy, &c1));
	if (between(0x00, c1, 0x7F)) {
		// One byte
		decode_single_byte(c1, out);
	} else if (between(0xC2, c1, 0xDF)) {
		// Two bytes
		uchar c2;
		check(get_next_uchar(str, &index_cpy, &c2));
		check(ensure_default(c2));
		decode_double_byte(c1, c2, out);
	} else if (between(0xE0, c1, 0xEF)) {
		// Three bytes
		uchar c2, c3;
		check(get_next_uchar(str, &index_cpy, &c2));
		check(get_next_uchar(str, &index_cpy, &c3));
		switch (c1) {
		case 0xE0:
			// Overlong Encoding
			r = ensure(0xA0, c2, 0xBF);
			if (!r.success) {
				if (between(0x80, c2, 0x9F)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (will result in an overlong encoding)",
						EUnicodeError, c2);
				}
			} else {
				return r;
			}
			break;
		case 0xED:
			// Surrogates
			r = ensure(0x80, c2, 0x9F);
			if (!r.success) {
				if (between(0xA0, c2, 0xBF)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X in (will result in a surrogate codepoint)",
						EUnicodeError, c2);
				} else {
					return r;
				}
			}
			break;
		default:
			check(ensure_default(c2));
			break;
		}
		check(ensure_default(c3));
		decode_triple_byte(c1, c2, c3, out);
	} else if (between(0xF0, c1, 0xF4)) {
		// Four bytes
		uchar c2, c3, c4;
		check(get_next_uchar(str, &index_cpy, &c2));
		check(get_next_uchar(str, &index_cpy, &c3));
		check(get_next_uchar(str, &index_cpy, &c4));
		switch (c1) {
		case 0xF0:
			r = ensure(0x90, c2, 0xBF);
			if (!r.success) {
				if (between(0x80, c2, 0x8F)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (will result in an overlong encoding)",
						EUnicodeError, c2);
				}
			} else {
				return r;
			}
			break;
		case 0xF4:
			// Out of Bounds
			check(ensure(0x80, c2, 0x8F));
			break;
		default:
			check(ensure_default(c2));
			break;
		}
		check(ensure_default(c3));
		check(ensure_default(c4));
		decode_quadruple_byte(c1, c2, c3, c4, out);
	} else if (between(0xC0, c1, 0xC1)) {
		return new_errorf(
			"Invalid first byte 0x%2X (will result in an overlong encoding)",
			EUnicodeError, c1);
	} else if (between(0xF5, c1, 0xFF)) {
		return new_errorf(
			"Invalid first byte 0x%2X (codepoint out of Unicode range)",
			EUnicodeError, c1);
		} else {
		return new_errorf(
			"Invalid value for first byte of unicode codepoint 0x%2X (expected 0x00-0x7F, 0xC2-0xF4)",
			EUnicodeError, c1);
	}
	*index = index_cpy;
	return new_success();
}

/**
 * @brief Encodes a Unicode codepoint into UTF-8 and appends it to a string
 */
Result utf8_append_encoded_codepoint(UnicodeCodePoint cp, string *str) {
	const uchar b3 = b(111);
	const uchar b4 = b(1111);
	const uchar b5 = b(11111);
	const uchar b6 = b(111111);
	const uchar cont_header = b(1) << 7;
	const uchar double_header = b(11) << 6;
	const uchar triple_header = b(111) << 5;
	const uchar quadruple_header = b(1111) << 4;

	if (btwcp(0, cp, 0x7F)) {
		// One byte
		string_append_uchar(str, cp);
	} else if (btwcp(0x0080, cp, 0x07FF)) {
		// Two bytes
		uchar byte1 = ((cp >> 6) & b5) + double_header;
		uchar byte2 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1);
		string_append_uchar(str, byte2);
	} else if (btwcp(0x0800, cp, 0xD7FF) || btwcp(0xE000, cp, 0xFFFF)) {
		// Three bytes (skipping surrogate range)
		uchar byte1 = ((cp >> 12) & b4) + triple_header;
		uchar byte2 = ((cp >> 6) & b6) + cont_header;
		uchar byte3 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1);
		string_append_uchar(str, byte2);
		string_append_uchar(str, byte3);
	} else if (btwcp(0x10000, cp, 0x10FFFF)) {
		// Four bytes
		uchar byte1 = ((cp >> 18) & b3) + quadruple_header;
		uchar byte2 = ((cp >> 12) & b6) + cont_header;
		uchar byte3 = ((cp >> 6) & b6) + cont_header;
		uchar byte4 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1);
		string_append_uchar(str, byte2);
		string_append_uchar(str, byte3);
		string_append_uchar(str, byte4);
	} else if (btwcp(0xD800, cp, 0xDBFF)) {
		return new_errorf(
			"Tried to encode invalid Unicode codepoint 0x%2X: UTF-16 Surrogate High Half",
			EUnicodeError, cp);
	} else if (btwcp(0xDC00, cp, 0xDFFF)) {
		return new_errorf(
			"Tried to encode invalid Unicode codepoint 0x%2X: UTF-16 Surrogate Low Half",
			EUnicodeError, cp);
	} else {
		return new_errorf("Tried to encode invalid Unicode codepoint 0x%2X", EUnicodeError, cp);
	}
	return new_success();
}
