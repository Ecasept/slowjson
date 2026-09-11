#pragma once
#include "internal/jsonvalue.h"
#include "utils/alloc/arena.h"

/* Owns the root and all its storage. Initialize before use; do not shallow-copy
 * a live document. Values passed to mutations must belong to this document.
 * Borrowed values/views expire on replacement, reset or free. Container
 * references can also move when their parent object/array grows. */
typedef struct JSONDocument {
    JSONValue value;
    Allocator value_allocator;
    Allocator string_allocator;
    Arena *string_arena;
} JSONDocument;

JSONDocument json_document_new(void);
void json_document_free(JSONDocument *doc);
void json_document_reset(JSONDocument *doc);
void json_document_set_object(JSONDocument *doc);
void json_document_set_array(JSONDocument *doc);
/* Output belongs to the caller; release with json_document_free_string. */
Result json_document_serialize(const JSONDocument *doc, string *out);
void json_document_free_string(const JSONDocument *doc, string *str);

/* Getters leave out unchanged on failure. Strings and container copies are
 * borrowed, not separately owned. Treat container copies as read-only. */
Result json_object_get_typed_sv(const JSONValue *obj, string_view key, JSONType type, JSONValue *out);
/* Borrow mutable references to existing values (no ownership transfer). */
Result json_object_get_ref_sv(JSONValue *obj, string_view key, JSONValue **out);
Result json_object_get_ref(JSONValue *obj, const char *key, JSONValue **out);
Result json_array_get_ref(JSONValue *array, size_t index, JSONValue **out);
Result json_object_get_string_sv(const JSONValue *obj, string_view key, string_view *out);
Result json_object_get_string(const JSONValue *obj, const char *key, string_view *out);
Result json_object_get_int_sv(const JSONValue *obj, string_view key, int64_t *out);
Result json_object_get_int(const JSONValue *obj, const char *key, int64_t *out);
Result json_object_get_cint_sv(const JSONValue *obj, string_view key, int *out);
Result json_object_get_cint(const JSONValue *obj, const char *key, int *out);
Result json_object_get_double_sv(const JSONValue *obj, string_view key, double *out);
Result json_object_get_double(const JSONValue *obj, const char *key, double *out);
Result json_object_get_bool_sv(const JSONValue *obj, string_view key, bool *out);
Result json_object_get_bool(const JSONValue *obj, const char *key, bool *out);
Result json_object_get_array_sv(const JSONValue *obj, string_view key, JSONValue *out);
Result json_object_get_array(const JSONValue *obj, const char *key, JSONValue *out);
Result json_object_get_object_sv(const JSONValue *obj, string_view key, JSONValue *out);
Result json_object_get_object(const JSONValue *obj, const char *key, JSONValue *out);

/* Setters copy keys/string values and replace existing fields. */
Result json_object_set_string_sv(JSONDocument *doc, JSONValue *obj, string_view key, string_view value);
Result json_object_set_string(JSONDocument *doc, JSONValue *obj, const char *key, const char * value);
Result json_object_set_int_sv(JSONDocument *doc, JSONValue *obj, string_view key, int64_t value);
Result json_object_set_int(JSONDocument *doc, JSONValue *obj, const char *key, int64_t value);
Result json_object_set_double_sv(JSONDocument *doc, JSONValue *obj, string_view key, double value);
Result json_object_set_double(JSONDocument *doc, JSONValue *obj, const char *key, double value);
Result json_object_set_bool_sv(JSONDocument *doc, JSONValue *obj, string_view key, bool value);
Result json_object_set_bool(JSONDocument *doc, JSONValue *obj, const char *key, bool value);
/* Container setters create empty containers; out may be NULL. */
Result json_object_set_object(JSONDocument *doc, JSONValue *obj, const char *key, JSONValue **out);
Result json_object_set_array(JSONDocument *doc, JSONValue *obj, const char *key, JSONValue **out);
Result json_object_set_null(JSONDocument *doc, JSONValue *obj, const char *key);
Result json_array_clear(JSONDocument *doc, JSONValue *array);
Result json_array_append_object(JSONDocument *doc, JSONValue *array, JSONValue **out);
Result json_array_append_string(JSONDocument *doc, JSONValue *array, const char *value);
