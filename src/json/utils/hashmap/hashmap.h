#pragma once
#include "../string/dstring.h"
#include "../string/string_view.h"

struct json_value_hashmap_node;
typedef struct json_value_hashmap_node json_value_hashmap_node;

// Include generic list
#define TYPE json_value_hashmap_node
#define TYPED_NAME(name) json_value_hashmap_node_##name
#define LIST_DECLARATION
#include "../list.h"
#undef LIST_DECLARATION
#undef TYPE
#undef TYPED_NAME

struct json_value_hashmap {
	json_value_hashmap_node_list buckets;
	size_t size;
};
typedef struct json_value_hashmap json_value_hashmap;
#include "../../internal/jsonvalue.h"

// ==== Functions ====

Result json_value_hashmap_get(const json_value_hashmap *map, string_view key,
							  JSONValue *out);

void json_value_hashmap_set(json_value_hashmap *map, string key,
                            JSONValue value, Allocator a);

void json_value_hashmap_init(json_value_hashmap *map, Allocator a);
void json_value_hashmap_free(json_value_hashmap *map, Allocator a);
