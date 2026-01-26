#include "utils.h"
#include <stdlib.h>
#include <wchar.h>
#include "utf8.h"

// ==== Important Data ===
// Encoding:
// https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/#G27288
// Well-Formed Byte Sequences:
// https://www.unicode.org/versions/Unicode17.0.0/core-spec/chapter-3/#G27506


UTF8Decoder utf8_decoder_new(string_view source) {
	return (UTF8Decoder) {
		.index = 0,
		.source = source
	};
}

/**
 * @brief Returns the next uchar from a string, or an IndexOutOfBounds Error
 */
static inline Result get_next_uchar(string_view str, size_t index, uchar *out) {
	if (index >= str.size) {
		return new_error(
			"Unexpected end of unicode string while reading codepoint",
			EUnicodeUnexpectedEndOfString);
	}
	*out = sv_at_unchecked(str, index);
	return new_success();
}

#define continuation_error(lower, byte, upper) \
	new_errorf( \
		"Invalid continuation byte 0x%2X. Expected between 0x%2X and 0x%2X", \
		EUnicodeError, byte, lower, upper)

/**
 * @brief Ensures the passed continuation bytes is in its valid range and
 * returns an appropriate error if not
 */
#define ensure(lower, c, upper) \
	if (!between(lower, c, upper)) { \
		return continuation_error(lower, c, upper); \
	} \

/**
 * @brief Ensure the unicode byte is in the default valid range for most
 * continuation bytes
 */
#define ensure_default(c) ensure(0x80, c, 0xBF)

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

Result utf8_get_next_codepoint(string_view str, size_t *index,
							   UnicodeCodePoint *out) {
	if (*index >= str.size) {
		return new_error(
			"Unexpected end of unicode string while reading codepoint",
			EUnicodeUnexpectedEndOfString);
	}
	uchar c1 = sv_at_unchecked(str, *index);
	if (between(0x00, c1, 0x7F)) {
		// One byte
		decode_single_byte(c1, out);
		*index += 1;
		return new_success();
	}

	if (between(0xC2, c1, 0xDF)) {
		// Two bytes
		if (*index + 1 >= str.size) {
			return new_error(
				"Unexpected end of unicode string while reading codepoint: expected 2 bytes but found 1",
				EUnicodeError);
		}
		uchar c2 = sv_at_unchecked(str, *index + 1);
		ensure_default(c2);
		decode_double_byte(c1, c2, out);
		*index += 2;
	} else if (between(0xE0, c1, 0xEF)) {
		// Three bytes
		if (*index + 2 >= str.size) {
			return new_errorf(
				"Unexpected end of unicode string while reading codepoint: expected 3 bytes but found %zu",
				EUnicodeError, str.size - *index);
		}
		uchar c2 = sv_at_unchecked(str, *index + 1);
		uchar c3 = sv_at_unchecked(str, *index + 2);
		switch (c1) {
		case 0xE0:
			// Overlong Encoding
			if (!between(0xA0, c2, 0xBF)) {
				if (between(0x80, c2, 0x9F)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (will result in an overlong encoding)",
						EUnicodeError, c2);
				} else {
					return continuation_error(0xA0, c2, 0xBF);
				}
			}
			break;
		case 0xED:
			// Surrogates
			if (!between(0x80, c2, 0x9F)) {
				if (between(0xA0, c2, 0xBF)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (will result in a surrogate codepoint)",
						EUnicodeError, c2);
				} else {
					return continuation_error(0x80, c2, 0x9F);
				}
			}
			break;

		default:
			ensure_default(c2);
			break;
		}
		ensure_default(c3);
		decode_triple_byte(c1, c2, c3, out);
		*index += 3;
	} else if (between(0xF0, c1, 0xF4)) {
		if (*index + 3 >= str.size) {
			return new_errorf(
				"Unexpected end of unicode string while reading codepoint: expected 4 bytes but found %zu",
				EUnicodeError, str.size - *index);
		}
		// Four bytes
		uchar c2 = sv_at_unchecked(str, *index + 1);
		uchar c3 = sv_at_unchecked(str, *index + 2);
		uchar c4 = sv_at_unchecked(str, *index + 3);
		switch (c1) {
		case 0xF0:
			if (!between(0x90, c2, 0xBF)) {
				if (between(0x80, c2, 0x8F)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (will result in an overlong encoding)",
						EUnicodeError, c2);
				} else {
					return continuation_error(0x90, c2, 0xBF);
				}
			}
			break;
		case 0xF4:
			// Out of Bounds
			if (!between(0x80, c2, 0x8F)) {
				if (between(0x90, c2, 0xBF)) {
					return new_errorf(
						"Invalid continuation byte 0x%2X (codepoint out of Unicode range)",
						EUnicodeError, c2);
				} else {
					return continuation_error(0x80, c2, 0x8F);
				}
			}
			break;
		default:
			ensure_default(c2);
			break;
		}
		ensure_default(c3);
		ensure_default(c4);
		decode_quadruple_byte(c1, c2, c3, c4, out);
		*index += 4;
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
	return new_success();
}

/**
 * @brief Encodes a Unicode codepoint into UTF-8 and appends it to a string
 */
Result utf8_append_encoded_codepoint(UnicodeCodePoint cp, string *str, Allocator a) {
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
		string_append_uchar(str, cp, a);
	} else if (btwcp(0x0080, cp, 0x07FF)) {
		// Two bytes
		uchar byte1 = ((cp >> 6) & b5) + double_header;
		uchar byte2 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1, a);
		string_append_uchar(str, byte2, a);
	} else if (btwcp(0x0800, cp, 0xD7FF) || btwcp(0xE000, cp, 0xFFFF)) {
		// Three bytes (skipping surrogate range)
		uchar byte1 = ((cp >> 12) & b4) + triple_header;
		uchar byte2 = ((cp >> 6) & b6) + cont_header;
		uchar byte3 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1, a);
		string_append_uchar(str, byte2, a);
		string_append_uchar(str, byte3, a);
	} else if (btwcp(0x10000, cp, 0x10FFFF)) {
		// Four bytes
		uchar byte1 = ((cp >> 18) & b3) + quadruple_header;
		uchar byte2 = ((cp >> 12) & b6) + cont_header;
		uchar byte3 = ((cp >> 6) & b6) + cont_header;
		uchar byte4 = (cp & b6) + cont_header;
		string_append_uchar(str, byte1, a);
		string_append_uchar(str, byte2, a);
		string_append_uchar(str, byte3, a);
		string_append_uchar(str, byte4, a);
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
