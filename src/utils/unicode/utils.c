#include "utils.h"

/**
 * @brief Returns the next uchar from a string, or an IndexOutOfBounds Error
 */
Result _get_next_uchar(const string *str, size_t *index, uchar *out) {
	uchar c;
	Result r;
	if ((r = string_at_err(str, *index, &c)).success == false) {
		switch (r.type) {
		case EIndexOutOfBounds:
			return new_error(
				"Unexpected end of unicode string while reading codepoint",
				EUnicodeUnexpectedEndOfString);
		default:
			return r;
		}
	}
	*out = c;
	*index += 1;
	return new_success();
}

bool is_surrogate_half(UnicodeCodePoint cp) {
	return cp >= 0xD800 && cp <= 0xDFFF;
}
bool is_low_surrogate(UnicodeCodePoint cp) {
	return cp >= 0xDC00 && cp <= 0xDFFF;
}
bool is_high_surrogate(UnicodeCodePoint cp) {
	return cp >= 0xD800 && cp <= 0xDBFF;
}

UnicodeCodePoint decode_surrogate_pair(UnicodeCodePoint high,
									   UnicodeCodePoint low) {
	return 0x10000 + (((high - 0xD800) << 10) | (low - 0xDC00));
}
