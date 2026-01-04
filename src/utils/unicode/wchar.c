#include "wchar.h"
#include "utf8.h"
#include "utf16.h"

/**
 * @brief Converts a wchar_t string to a UTF-8 encoded string
 * 
 * @param src The source wchar_t string (will be borrowed)
 * @param dest Pointer to store the resulting UTF-8 string (will be allocated)
 */
Result wchar_to_utf8_string(const wchar_t *src, string *dest) {
	if (sizeof(wchar_t) == 2) {
		// UTF-16
		size_t index = 0;
		Result r;
		UnicodeCodePoint cp;

		string_new(dest, "");

		while (src[index] != L'\0') {
			wchar_t w1 = src[index];

			if (is_high_surrogate(w1)) {
				// Need to read another wchar_t
				wchar_t w2 = src[index + 1];
				if (!is_low_surrogate(w2)) {
					string_free(dest);
					return new_errorf(
						"Invalid UTF-16 sequence: expected low surrogate after high surrogate at index %zu",
						EUnicodeError, index);
				}
				cp = decode_surrogate_pair(w1, w2);
				index++; // Advance extra for surrogate pair
			} else if (is_low_surrogate(w1)) {
				string_free(dest);
				return new_errorf(
					"Invalid UTF-16 sequence: unexpected low surrogate at index %zu",
					EUnicodeError, index);
			} else {
				cp = (UnicodeCodePoint)w1;
			}

			r = utf8_append_encoded_codepoint(cp, dest);
			if (!r.success) {
				string_free(dest);
				return r;
			}
			index++;
		}
		return new_success();
	} else if (sizeof(wchar_t) == 4) {
		// UTF-32
		size_t index = 0;
		Result r;
		string_new(dest, "");

		while (src[index] != L'\0') {
			UnicodeCodePoint cp = (UnicodeCodePoint)src[index];
			r = utf8_append_encoded_codepoint(cp, dest);
			if (!r.success) {
				string_free(dest);
				return r;
			}
			index++;
		}
		return new_success();
	} else {
		panicf("Unsupported wchar_t size: %zu bytes", sizeof(wchar_t));
	}
}


/**
 * @brief Converts a UTF-8 encoded string to a wchar_t string
 * @param src The source UTF-8 string
 * @param dest Pointer to store the resulting wchar_t string (will be allocated)
 */
Result utf8_string_to_wchar(string_view src, wchar_t **dest) {

	wchar_list out;
	wchar_list_init(&out, 0);

	if (sizeof(wchar_t) == 2) {
		// UTF-16
		size_t index = 0;
		Result r;
		UnicodeCodePoint cp;
		while (index < src.size) {
			r = utf8_get_next_codepoint(src, &index, &cp);
			if (!r.success) {
				wchar_list_free(&out);
				return r;
			}
			if (needs_surrogate_pair(cp)) {
				UCP high, low;
				encode_surrogate_pair(cp, &high, &low);
				wchar_list_push(&out, (wchar_t)high);
				wchar_list_push(&out, (wchar_t)low);
			} else {
				wchar_list_push(&out, (wchar_t)cp);
			}
		}
		wchar_list_push(&out, L'\0');
		*dest = out.data;
		return new_success();
	} else if (sizeof(wchar_t) == 4) {
		// UTF-32
		Result r;
		UCP cp;
		size_t index = 0;

		while (index < src.size) {
			r = utf8_get_next_codepoint(src, &index, &cp);
			if (!r.success) {
				wchar_list_free(&out);
				return r;
			}
			wchar_list_push(&out, (wchar_t)cp);
		}
		wchar_list_push(&out, L'\0');
		*dest = out.data;
		return new_success();
	} else {
		panicf("Unsupported wchar_t size: %zu bytes", sizeof(wchar_t));
	}
}


#define TYPE wchar_t
#define TYPED_NAME(name) wchar_##name
#define LIST_IMPLEMENTATION
#include "../../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
