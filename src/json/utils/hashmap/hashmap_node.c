#include "hashmap_node.h"
#include "../custom_error.h"
#include "hashmap.h"

Result json_value_hashmap_node_get(json_value_hashmap_node *node,
								   string_view key, JSONValue *out) {
	if (string_eq_sv(&node->key, key)) {
		*out = node->value;
		return new_success();
	} else {
		if (node->next == NULL) {
			// End reached
			Result r = new_errorf("Key not found in hashmap: \"%.*s\"",
								  EHashmapKeyNotFound, (int)key.size, key.data);
			return r;
		} else {
			return json_value_hashmap_node_get(node->next, key, out);
		}
	}
}

bool json_value_hashmap_node_set(json_value_hashmap_node *node, string key,
                                 JSONValue value, Allocator a) {
	bool eq;
	string_eq(&node->key, &key, &eq);
	if (eq) {
		// Overwrite existing value
		json_value_free(&node->value, a);
		string_free(&node->key, a);
		node->key = key;
		node->value = value;
		return false;
	} else {
		if (node->next == NULL) {
			// End reached
			json_value_hashmap_node *node2 = alloc(a, sizeof(json_value_hashmap_node));
			if (node2 == NULL) {
				panic("Failed to allocate memory for hashmap node");
			}
			node2->key = key;
			node2->value = value;
			node2->next = NULL;
			node->next = node2;
			return true;
		} else {
			return json_value_hashmap_node_set(node->next, key, value, a);
		}
	}
}

/**
 * @brief Frees the successor nodes. Does not free the node itself.
 */
void json_value_hashmap_node_free(json_value_hashmap_node *node, Allocator a) {
	string_free(&node->key, a);
	json_value_free(&node->value, a);
	if (node->next != NULL) {
		json_value_hashmap_node_free(node->next, a);
		dealloc(a, node->next);
	}
}
