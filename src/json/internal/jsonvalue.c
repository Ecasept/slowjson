#include "jsonvalue.h"

static const char *json_type_strings[] = {
	"null",
	"bool",
	"number",
	"string",
	"array",
	"object",
};

const char *jtostr(JSONType type) {
	if (type < 0 || type >= (int)(sizeof(json_type_strings) / sizeof(json_type_strings[0]))) {
		return "unknown";
	}
	return json_type_strings[type];
}

void json_value_free(JSONValue *value, Allocator a) {
	json_value_free_split(value, a, a);
}

void json_value_free_split(JSONValue *value, Allocator a, Allocator stra) {
	switch (value->type) {
	case JSON_NULL:
	case JSON_BOOL:
	case JSON_NUMBER:
		// does not need to be freed
		break;
	case JSON_ARRAY:
		for (size_t i = 0; i < value->list.length; i++) {
			json_value_free_split(&value->list.data[i], a, stra);
		}
		json_value_list_free(&value->list, a);
		break;
	case JSON_OBJECT:
		json_value_hashmap_free_split(&value->hashmap, a, stra);
		break;
	case JSON_STRING:
		string_free(&value->str, stra);
		break;
	}
}

JSONValue json_value_new_null(void) {
	JSONValue val;
	val.type = JSON_NULL;
	return val;
}

JSONValue json_value_new_bool(bool b) {
	JSONValue val;
	val.type = JSON_BOOL;
	val.boolean = b;
	return val;
}

JSONValue json_value_new_number(JSONNumber number) {
	JSONValue val;
	val.type = JSON_NUMBER;
	val.number = number;
	return val;
}

JSONValue json_value_new_integer(int64_t int_value) {
	JSONValue val;
	val.type = JSON_NUMBER;
	val.number.is_integer = true;
	val.number.int_value = int_value;
	return val;
}

JSONValue json_value_new_float(double float_value) {
	JSONValue val;
	val.type = JSON_NUMBER;
	val.number.is_integer = false;
	val.number.float_value = float_value;
	return val;
}

JSONValue json_value_new_string(string *str) {
	JSONValue val;
	val.type = JSON_STRING;
	val.str = *str;
	return val;
}

JSONValue json_value_new_string_cstr(const char *str, Allocator a) {
	JSONValue val;
	val.type = JSON_STRING;
	string_new(&val.str, str, a);
	return val;
}

JSONValue json_value_new_array(Allocator a) {
	JSONValue val;
	val.type = JSON_ARRAY;
	json_value_list_init(&val.list, 0, a);
	return val;
}
JSONValue json_value_new_object(Allocator a) {
	JSONValue val;
	val.type = JSON_OBJECT;
	json_value_hashmap_init(&val.hashmap, a);
	return val;
}

#define TYPE JSONValue
#define TYPED_NAME(name) json_value_##name
#define LIST_IMPLEMENTATION
#include "../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
