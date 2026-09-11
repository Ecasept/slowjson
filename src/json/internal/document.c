#include "../document.h"
#include "../serialize.h"
#include "../utils/alloc/default.h"
#include <limits.h>

JSONDocument json_document_new(void) {
    JSONDocument doc = {0};
    doc.value = json_value_new_null();
    doc.value_allocator = ga;
    doc.string_arena = malloc(sizeof(*doc.string_arena));
    if (!doc.string_arena) panic("Failed to allocate document arena");
    *doc.string_arena = new_arena();
    doc.string_allocator = arena_as_allocator(doc.string_arena);
    return doc;
}

void json_document_free(JSONDocument *doc) {
    if (!doc->string_arena) return;
    json_value_free_split(&doc->value, doc->value_allocator, doc->string_allocator);
    arena_free(doc->string_arena);
    free(doc->string_arena);
    *doc = (JSONDocument){0};
}

void json_document_reset(JSONDocument *doc) {
    json_document_free(doc);
    *doc = json_document_new();
}

void json_document_set_object(JSONDocument *doc) {
    json_document_reset(doc);
    doc->value = json_value_new_object(doc->value_allocator);
}

void json_document_set_array(JSONDocument *doc) {
    json_document_reset(doc);
    doc->value = json_value_new_array(doc->value_allocator);
}

Result json_document_serialize(const JSONDocument *doc, string *out) {
    return json_serialize(&doc->value, out, doc->value_allocator);
}

void json_document_free_string(const JSONDocument *doc, string *str) {
    string_free(str, doc->value_allocator);
}

Result json_object_get_typed_sv(const JSONValue *obj, string_view key, JSONType type, JSONValue *out) {
    if (obj->type != JSON_OBJECT) return new_error("Expected JSON object", ESaveFormatError);
    JSONValue value = {0};
    check(json_value_hashmap_get(&obj->hashmap, key, &value));
    if (value.type != type) {
        return new_errorf("Field '%.*s': expected %s, got %s", ESaveFormatError,
                          (int)key.size, key.data, jtostr(type), jtostr(value.type));
    }
    *out = value;
    return new_success();
}

Result json_object_get_ref_sv(JSONValue *obj, string_view key, JSONValue **out) {
    if (obj->type != JSON_OBJECT) return new_error("Expected JSON object", ESaveFormatError);
    return json_value_hashmap_get_ref(&obj->hashmap, key, out);
}

Result json_object_get_ref(JSONValue *obj, const char *key, JSONValue **out) {
    return json_object_get_ref_sv(obj, as_svc(key), out);
}

Result json_array_get_ref(JSONValue *array, size_t index, JSONValue **out) {
    if (array->type != JSON_ARRAY) return new_error("Expected JSON array", ESaveFormatError);
    if (index >= array->list.length) return new_error("Array index out of bounds", EIndexOutOfBounds);
    *out = &array->list.data[index];
    return new_success();
}

Result json_object_get_string_sv(const JSONValue *obj, string_view key, string_view *out) {
    JSONValue value = {0};
    check(json_object_get_typed_sv(obj, key, JSON_STRING, &value));
    *out = as_sv(value.str);
    return new_success();
}

Result json_object_get_int_sv(const JSONValue *obj, string_view key, int64_t *out) {
    JSONValue value = {0};
    check(json_object_get_typed_sv(obj, key, JSON_NUMBER, &value));
    if (!value.number.is_integer) return new_error("Expected JSON integer", ESaveFormatError);
    *out = value.number.int_value;
    return new_success();
}

Result json_object_get_cint_sv(const JSONValue *obj, string_view key, int *out) {
    int64_t value = 0;
    check(json_object_get_int_sv(obj, key, &value));
    if (value < INT_MIN || value > INT_MAX) return new_error("Integer out of C int range", ESaveFormatError);
    *out = (int)value;
    return new_success();
}

Result json_object_get_double_sv(const JSONValue *obj, string_view key, double *out) {
    JSONValue value = {0};
    check(json_object_get_typed_sv(obj, key, JSON_NUMBER, &value));
    *out = value.number.is_integer ? (double)value.number.int_value : value.number.float_value;
    return new_success();
}

Result json_object_get_bool_sv(const JSONValue *obj, string_view key, bool *out) {
    JSONValue value = {0};
    check(json_object_get_typed_sv(obj, key, JSON_BOOL, &value));
    *out = value.boolean;
    return new_success();
}

Result json_object_get_array_sv(const JSONValue *obj, string_view key, JSONValue *out) {
    return json_object_get_typed_sv(obj, key, JSON_ARRAY, out);
}

Result json_object_get_object_sv(const JSONValue *obj, string_view key, JSONValue *out) {
    return json_object_get_typed_sv(obj, key, JSON_OBJECT, out);
}

Result json_object_get_string(const JSONValue *obj, const char *key, string_view *out) {
    return json_object_get_string_sv(obj, as_svc(key), out);
}

Result json_object_get_int(const JSONValue *obj, const char *key, int64_t *out) {
    return json_object_get_int_sv(obj, as_svc(key), out);
}

Result json_object_get_cint(const JSONValue *obj, const char *key, int *out) {
    return json_object_get_cint_sv(obj, as_svc(key), out);
}

Result json_object_get_double(const JSONValue *obj, const char *key, double *out) {
    return json_object_get_double_sv(obj, as_svc(key), out);
}

Result json_object_get_bool(const JSONValue *obj, const char *key, bool *out) {
    return json_object_get_bool_sv(obj, as_svc(key), out);
}

Result json_object_get_array(const JSONValue *obj, const char *key, JSONValue *out) {
    return json_object_get_array_sv(obj, as_svc(key), out);
}

Result json_object_get_object(const JSONValue *obj, const char *key, JSONValue *out) {
    return json_object_get_object_sv(obj, as_svc(key), out);
}

static Result require_object(const JSONValue *obj) {
    return obj->type == JSON_OBJECT ? new_success() : new_error("Expected JSON object", ESaveFormatError);
}

static void set_value(JSONDocument *doc, JSONValue *obj, string_view key, JSONValue value) {
    string owned_key;
    string_from_view(&owned_key, key, doc->string_allocator);
    json_value_hashmap_set_split(&obj->hashmap, owned_key, value,
                                doc->value_allocator, doc->string_allocator);
}

Result json_object_set_string_sv(JSONDocument *doc, JSONValue *obj, string_view key, string_view value) {
    check(require_object(obj));
    string owned;
    string_from_view(&owned, value, doc->string_allocator);
    set_value(doc, obj, key, json_value_new_string(&owned));
    return new_success();
}

Result json_object_set_string(JSONDocument *doc, JSONValue *obj, const char *key, const char *value) {
    return json_object_set_string_sv(doc, obj, as_svc(key), as_svc(value));
}

Result json_object_set_int_sv(JSONDocument *doc, JSONValue *obj, string_view key, int64_t value) {
    check(require_object(obj));
    set_value(doc, obj, key, json_value_new_integer(value));
    return new_success();
}
Result json_object_set_int(JSONDocument *doc, JSONValue *obj, const char *key, int64_t value) {
    return json_object_set_int_sv(doc, obj, as_svc(key), value);
}

Result json_object_set_double_sv(JSONDocument *doc, JSONValue *obj, string_view key, double value) {
    check(require_object(obj));
    set_value(doc, obj, key, json_value_new_float(value));
    return new_success();
}
Result json_object_set_double(JSONDocument *doc, JSONValue *obj, const char *key, double value) {
    return json_object_set_double_sv(doc, obj, as_svc(key), value);
}

Result json_object_set_bool_sv(JSONDocument *doc, JSONValue *obj, string_view key, bool value) {
    check(require_object(obj));
    set_value(doc, obj, key, json_value_new_bool(value));
    return new_success();
}
Result json_object_set_bool(JSONDocument *doc, JSONValue *obj, const char *key, bool value) {
    return json_object_set_bool_sv(doc, obj, as_svc(key), value);
}

Result json_object_set_object(JSONDocument *doc, JSONValue *obj, const char *key, JSONValue **out) {
    check(require_object(obj));
    set_value(doc, obj, as_svc(key), json_value_new_object(doc->value_allocator));
    if (out) return json_value_hashmap_get_ref(&obj->hashmap, as_svc(key), out);
    return new_success();
}

Result json_object_set_array(JSONDocument *doc, JSONValue *obj, const char *key, JSONValue **out) {
    check(require_object(obj));
    set_value(doc, obj, as_svc(key), json_value_new_array(doc->value_allocator));
    if (out) return json_value_hashmap_get_ref(&obj->hashmap, as_svc(key), out);
    return new_success();
}

Result json_object_set_null(JSONDocument *doc, JSONValue *obj, const char *key) {
    check(require_object(obj));
    set_value(doc, obj, as_svc(key), json_value_new_null());
    return new_success();
}

Result json_array_clear(JSONDocument *doc, JSONValue *array) {
    if (array->type != JSON_ARRAY) return new_error("Expected JSON array", ESaveFormatError);
    json_value_free_split(array, doc->value_allocator, doc->string_allocator);
    *array = json_value_new_array(doc->value_allocator);
    return new_success();
}

Result json_array_append_object(JSONDocument *doc, JSONValue *array, JSONValue **out) {
    if (array->type != JSON_ARRAY) return new_error("Expected JSON array", ESaveFormatError);
    json_value_list_push(&array->list, json_value_new_object(doc->value_allocator), doc->value_allocator);
    if (out) *out = &array->list.data[array->list.length - 1];
    return new_success();
}

Result json_array_append_string(JSONDocument *doc, JSONValue *array, const char *value) {
    if (array->type != JSON_ARRAY) return new_error("Expected JSON array", ESaveFormatError);
    json_value_list_push(&array->list, json_value_new_string_cstr(value, doc->string_allocator), doc->value_allocator);
    return new_success();
}
