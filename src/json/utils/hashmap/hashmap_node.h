#pragma once
#include "../../internal/jsonvalue.h"
#include "../string/string_view.h"

struct json_value_hashmap_node {
	JSONValue value;
	string key;
	json_value_hashmap_node *next;
};
typedef struct json_value_hashmap_node json_value_hashmap_node;

// ==== Functions ====
Result json_value_hashmap_node_get(json_value_hashmap_node *node,
								   string_view key, JSONValue *out);
bool json_value_hashmap_node_set(json_value_hashmap_node *node, string key,
								 JSONValue value);
void json_value_hashmap_node_free(json_value_hashmap_node *node);
