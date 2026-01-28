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

static inline uchar sv_at_unchecked(string_view sv, size_t index) {
	#ifdef DEBUG
	if (index >= sv.size) {
		panicf("Tried to access string_view of size %zu at index %zu",
			   sv.size, index);
	}
	#endif
	return sv.data[index];
}

Result sv_at_checked(string_view sv, size_t index, uchar *out);
bool string_eq_sv(const string *str, string_view sv);
bool sv_eq(string_view sv1, string_view sv2);
bool sv_startswith(string_view sv, string_view prefix);
bool sv_endswith(string_view sv, string_view suffix);
string_view sv_substr_unchecked(string_view sv, size_t start, size_t length);
int sv_find(string_view sv, string_view substr);
