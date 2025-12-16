#include "dstring.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void string_free(string *str) { char_list_free(&str->arr); }
void string_new(string *str, const char *source) {
	size_t len = strlen(source);
	char_list_init(&str->arr, len);

	memcpy(str->arr.data, source, len);
	str->arr.length = len;
}

void string_append(string *str, const string *other) {
	char_list_extend(&str->arr, &other->arr);
}
void string_append_char(string *str, char other) {
	char_list_push(&str->arr, other);
}
void string_append_cstr(string *str, const char *cstr) {
	size_t len = strlen(cstr);
	char_list_ensure_resize(&str->arr, str->arr.length + len);
	memcpy(&str->arr.data[str->arr.length], cstr, len);
	str->arr.length += len;
}
void string_eq(const string *str, const string *other, bool *res) {
	if (str->arr.length != other->arr.length) {
		*res = false;
		return;
	}
	*res = memcmp(str->arr.data, other->arr.data, str->arr.length) == 0;
}
Result string_substr(const string *str, string *out, size_t start,
					 size_t length) {
	if (start + length > str->arr.length) {
		return new_error("Substring out of range", ESubstrOutOfRange);
	}
	char_list_init(&out->arr, length);
	memcpy(out->arr.data, &str->arr.data[start], length);
	out->arr.length = length;
	return new_success();
}

void string_to_cstr(const string *str, char **cstr) {
	*cstr = (char *)malloc(sizeof(char) * (str->arr.length + 1));
	if (*cstr == NULL) {
		panic("Failed to allocate memory for C string");
	}
	memcpy(*cstr, str->arr.data, str->arr.length);
	(*cstr)[str->arr.length] = '\0';
}

void string_at(const string *str, size_t index, char *out) {
	char_list_get(&str->arr, index, out);
}
