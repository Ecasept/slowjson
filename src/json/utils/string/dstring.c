#include "dstring.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void string_free(string *str) { uchar_list_free(&str->arr); }

void string_new(string *str, const char *source) {
	size_t len = strlen(source);
	uchar_list_init(&str->arr, len);

	memcpy(str->arr.data, source, len);
	str->arr.length = len;
}

string string_newr(const char *source) {
	string str;
	string_new(&str, source);
	return str;
}

/**
 * @brief Appends another string to this string.
 *
 * @param str The string to append to
 * @param other The string to append. It is not modified or freed.
 */
void string_append(string *str, const string *other) {
	uchar_list_extend(&str->arr, &other->arr);
}

/**
 * @brief Appends a single uchar to this string.
 * @warning If your string is utf-8 encoded, manually appending individual bytes
 * may lead to ill-formed sequences.
 *
 * @param str The string to append to
 * @param other The uchar to append
 */
void string_append_uchar(string *str, uchar other) {
	uchar_list_push(&str->arr, other);
}
/**
 * @brief Appends a C string to this string.
 * @warning If your string is utf-8 encoded, appending arbitrary C strings
 * may lead to ill-formed sequences. Special care should be taken when U+0000
 * bytes are involved.
 */
void string_append_cstr(string *str, const char *cstr) {
	size_t len = strlen(cstr);
	uchar_list_ensure_resize(&str->arr, str->arr.length + len);
	memcpy(&str->arr.data[str->arr.length], cstr, len);
	str->arr.length += len;
}
/**
 * @brief Compares two strings for equality.
 * @note This is a bytewise comparison and does not take encoding into account.
 * This function does not test for canonical equivalence. In other words,
 * different code point sequences mapping to the same abstract character will be
 * considered unequal.
 */
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
	uchar_list_init(&out->arr, length);
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

void string_at(const string *str, size_t index, uchar *out) {
	uchar_list_get(&str->arr, index, out);
}

Result string_at_err(const string *str, size_t index, uchar *out) {
	return uchar_list_get_err(&str->arr, index, out);
}

void string_clone(const string *str, string *out) {
	uchar_list_init(&out->arr, str->arr.length);
	memcpy(out->arr.data, str->arr.data, str->arr.length);
	out->arr.length = str->arr.length;
}

bool string_eq_cstr(const string *str, const char *cstr) {
    size_t len = strlen(cstr);
    if (str->arr.length != len) return false;
    return memcmp(str->arr.data, cstr, len) == 0;
}
