#include "utils.h"

/**
 * @brief Returns the next uchar from a string, or an IndexOutOfBounds Error
 */
Result get_next_uchar(const string *str, size_t *index, uchar *out) {
	uchar c;
	Result r;
	if ((r = string_at_err(str, *index, &c)).success == false) {
		switch (r.type) {
		case EIndexOutOfBounds:
			error_free(r);
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
