#include "string_view.h"
#include "string.h"
#include <stdlib.h>

uchar sv_at_unchecked(string_view sv, size_t index) {
	#ifdef DEBUG
	if (index >= sv.size) {
		panicf("Tried to access string_view of size %zu at index %zu",
			   sv.size, index);
	}
	#endif
	return sv.data[index];
}

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
