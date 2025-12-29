#pragma once

#include "custom_error.h"
#include "unicode/unicode_types.h"

#define TYPE uchar
#define TYPED_NAME(name) uchar_##name
#define LIST_DECLARATION
#include "list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

struct string {
	uchar_list arr;
};
typedef struct string string;

void string_free(string *str);
void string_new(string *str, const char *source);

void string_append(string *str, const string *other);
void string_append_uchar(string *str, uchar other);
void string_append_cstr(string *str, const char *cstr);
void string_eq(const string *str, const string *other, bool *res);
Result string_substr(const string *str, string *out, size_t start,
					 size_t length);

void string_to_cstr(const string *str, char **cstr);

void string_at(const string *str, size_t index, uchar *out);
Result string_at_err(const string *str, size_t index, uchar *out);
