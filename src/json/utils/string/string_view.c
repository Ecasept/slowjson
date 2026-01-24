#include "string_view.h"
#include "string.h"
#include <stdlib.h>
#include <limits.h>

Result sv_at_checked(string_view sv, size_t index, uchar *out) {
	if (index >= sv.size) {
		return new_errorf("Index %zu out of bounds for string_view of size %zu",
						  EIndexOutOfBounds, index, sv.size);
	}
	*out = sv.data[index];
	return new_success();
}
bool string_eq_sv(const string *str, string_view sv) {
	if (str->arr.length != sv.size) return false;
	return memcmp(str->arr.data, sv.data, str->arr.length) == 0;
}
bool sv_eq(string_view sv1, string_view sv2) {
	if (sv1.size != sv2.size) return false;
	return memcmp(sv1.data, sv2.data, sv1.size) == 0;
}
bool sv_startswith(string_view sv, string_view prefix) {
	if (sv.size < prefix.size) return false;
	return sv_eq(sv_substr_unchecked(sv, 0, prefix.size), prefix);
}
string_view sv_substr_unchecked(string_view sv, size_t start, size_t length) {
	#ifdef DEBUG
	if (start + length > sv.size) {
		panicf("Tried to get substring of string_view of size %zu "
			   "from %zu with length %zu",
			   sv.size, start, length);
	}
	#endif
	string_view substr = {
		.data = sv.data + start,
		.size = length
	};
	return substr;
}
bool sv_endswith(string_view sv, string_view suffix) {
	if (sv.size < suffix.size) return false;
	return sv_eq(sv_substr_unchecked(sv, sv.size - suffix.size, suffix.size), suffix);
}

static const size_t ALPHABET_SIZE = UCHAR_MAX + 1;

static void build_bad_char_table(size_t bad_char[ALPHABET_SIZE], string_view substr) {
	size_t m = substr.size;
	for (size_t i = 0; i < m; i++) {
		bad_char[i] = m;
	}
	for (size_t i = 0; i < m; i++) {
		bad_char[sv_at_unchecked(substr, i)] = i;
	}
}

#define max(a, b) ((a) > (b) ? (a) : (b))

int sv_find(string_view sv, string_view substr) {
	// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
	#ifdef __STDC_NO_VLA__
	#error "Variable Length Arrays are required for this function"
	#endif
	size_t bad_char[ALPHABET_SIZE];
	size_t good_suffix_L[substr.size];
	size_t good_suffix_H[substr.size];
	build_bad_char_table(bad_char, substr);
	

	size_t i = substr.size - 1;
while (i < sv.size) {
		size_t back = 0;
		while (back < substr.size) {
			size_t sv_idx = i - back;
			size_t substr_idx = substr.size - 1 - back;
			uchar substr_char = sv_at_unchecked(substr, substr_idx);
			uchar sv_char = sv_at_unchecked(sv, sv_idx);
			if (sv_char != substr_char) {
				size_t bad_char_shift = bad_char[substr_char];
				i += max(1, substr_idx - bad_char_shift);
				goto continue_outer_while;
			}
			back++;
		}
		return i;
continue_outer_while:
	}
	return -1;
}
