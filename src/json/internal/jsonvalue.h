#pragma once
#include "../../utils/dstring.h"
#include "stdbool.h"

enum JSONType {
	JSON_NULL,
	JSON_BOOL,
	JSON_NUMBER,
	JSON_STRING,
	JSON_ARRAY,
	JSON_OBJECT
};
typedef enum JSONType JSONType;

struct JSONValue;
typedef struct JSONValue JSONValue;

#define TYPE JSONValue
#define TYPED_NAME(name) json_value_##name
#define LIST_DECLARATION
#include "../../utils/list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

#include "../hashmap/hashmap.h"

struct JSONNumber {
	bool is_integer;
	union {
		int64_t int_value;
		double float_value;
	};
};
typedef struct JSONNumber JSONNumber;

struct JSONValue {
	JSONType type;
	union {
		bool boolean;
		JSONNumber number;
		string str;
		json_value_list list;
		json_value_hashmap hashmap;
	};
};

const char *jtostr(JSONType type);

void json_value_free(JSONValue *value);

JSONValue json_value_new_null();
JSONValue json_value_new_bool(bool b);
JSONValue json_value_new_number(JSONNumber number);
JSONValue json_value_new_integer(int64_t int_value);
JSONValue json_value_new_float(double float_value);
JSONValue json_value_new_string_cstr(const char *str);
JSONValue json_value_new_string(string *str);
JSONValue json_value_new_array();
JSONValue json_value_new_object();
