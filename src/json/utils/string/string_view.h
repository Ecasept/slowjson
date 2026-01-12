#pragma once
#include "../unicode/unicode_types.h"
#include "stdlib.h"
#include "string_struct.h"

struct string_view {
	const uchar* data;
	size_t size;
};
typedef struct string_view string_view;

#define svl(s) ((string_view){ .data = (const uchar*)(s), .size = sizeof((s)) - 1 })
static inline string_view as_sv(string str) {
	string_view sv;
	sv.data = str.arr.data;
	sv.size = str.arr.length;
	return sv;
}
static inline string_view as_svc(const char cstr[]) {
	string_view sv;
	sv.data = (const uchar*)cstr;
	sv.size = strlen(cstr);
	return sv;
}
uchar sv_at_unchecked(string_view sv, size_t index);
Result sv_at_checked(string_view sv, size_t index, uchar *out);
bool string_eq_sv(const string *str, string_view sv);
