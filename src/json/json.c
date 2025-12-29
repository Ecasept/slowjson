#include "json.h"

void json_value_free(JSONValue *value) {
	switch (value->type) {
	case JSON_NULL:
	case JSON_BOOL:
	case JSON_NUMBER:
		// does not need to be freed
		break;
	case JSON_ARRAY:
		json_value_list_free(&value->list);
		break;
	case JSON_OBJECT:
		json_value_hashmap_free(&value->hashmap);
		break;
	case JSON_STRING:
		string_free(&value->str);
		break;
	}
}

JSONValue json_value_new_null() {
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

JSONValue json_value_new_string(string *str) {
	JSONValue val;
	val.type = JSON_STRING;
	val.str = *str;
	return val;
}

JSONValue json_value_new_array() {
	JSONValue val;
	val.type = JSON_ARRAY;
	json_value_list_init(&val.list, 0);
	return val;
}
JSONValue json_value_new_object() {
	JSONValue val;
	val.type = JSON_OBJECT;
	json_value_hashmap_init(&val.hashmap);
	return val;
}

#define TYPE JSONValue
#define TYPED_NAME(name) json_value_##name
#define LIST_IMPLEMENTATION
#include "../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
