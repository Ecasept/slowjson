#include "hashmap.h"
#include "hashmap_node.h"

const size_t _HASHMAP_INITIAL_SIZE = 7;
const size_t HASH_PRIME = 53;
const double MAX_LOAD_FACTOR = 3;
const double MIN_LOAD_FACTOR = MAX_LOAD_FACTOR / 4;

size_t _string_hash(const string *str, size_t size) {
	// Implements a simple polynomial rolling hash function
	// https://cp-algorithms.com/string/string-hashing.html
	size_t hash = 0;
	size_t pow = 1;
	for (size_t i = 0; i < str->arr.length; i++) {
		uchar v;
		string_at(str, i, &v);
		hash += (v * pow) % size;
		pow *= HASH_PRIME;
	}
	return hash;
}

double _load_factor(const json_value_hashmap *map) {
	return (double)map->size / (double)map->buckets.length;
}

void json_value_hashmap_init(json_value_hashmap *map) {
	json_value_hashmap_node_list_init(&map->buckets, _HASHMAP_INITIAL_SIZE);
}

Result json_value_hashmap_get(const json_value_hashmap *map, const string *key,
							  JSONValue *out) {
	size_t node_index = _string_hash(key, map->buckets.length);
	json_value_hashmap_node node;
	Result r =
		json_value_hashmap_node_list_get_err(&map->buckets, node_index, &node);
	if (r.type == EIndexOutOfBounds) {
		error_free(r);
		return new_errorf("Key not found", EHashmapKeyNotFound);
	} else if (!r.success) {
		return r;
	}
	return json_value_hashmap_node_get(&node, key, out);
}

void _rehash(json_value_hashmap *map, size_t new_bucket_count) {
	size_t old_bucket_count = map->buckets.length;
	json_value_hashmap_node_list old_buckets = map->buckets;

	json_value_hashmap_node_list_init(&map->buckets, new_bucket_count);
	memset(map->buckets.data, NULL, new_bucket_count);
	map->buckets.length = new_bucket_count;
	for (int bucket = 0; bucket < old_bucket_count; bucket++) {
		json_value_hashmap_node node;
		json_value_hashmap_node_list_get(&old_buckets, bucket, &node);
		while (node.key != NULL) {
			_json_value_hashmap_set_internal(map, node.key, node.value, false);
		}
	}
	json_value_hashmap_node_list_free(&old_buckets);
}

void json_value_hashmap_set(json_value_hashmap *map, string *key,
							JSONValue value) {
	_json_value_hashmap_set_internal(map, key, value, true);
}

void _json_value_hashmap_set_internal(json_value_hashmap *map, string *key,
									  JSONValue value, bool rehash) {
	size_t load_factor = _load_factor(map);
	if (rehash && load_factor > MAX_LOAD_FACTOR) {
		_rehash(map, map->buckets.length * 2);
	} else if (rehash && load_factor < MIN_LOAD_FACTOR) {
		_rehash(map, map->buckets.length / 2);
	}
	size_t node_index = _string_hash(key, map->buckets.length);
	json_value_hashmap_node node;
	Result r =
		json_value_hashmap_node_list_get_err(&map->buckets, node_index, &node);
	if (r.type == EIndexOutOfBounds) {
		error_free(r);
		// Create new node
		json_value_hashmap_node new_node;
		new_node.key = key;
		new_node.value = value;
		new_node.next = NULL;
		json_value_hashmap_node_list_set(&map->buckets, node_index, new_node);
		map->size += 1;
	} else if (!r.success) {
		panicf("Failed to set hashmap node: %s", r.message);
	} else {
		// Update existing node
		if (json_value_hashmap_node_set(&node, key, value)) {
			// New entry added instead of overwritten
			map->size += 1;
		}
	}
}

void json_value_hashmap_free(json_value_hashmap *map) {
	for (size_t i = 0; i < map->buckets.length; i++) {
		json_value_hashmap_node node;
		json_value_hashmap_node_list_get(&map->buckets, i, &node);
		json_value_hashmap_node_free(&node);
	}
	json_value_hashmap_node_list_free(&map->buckets);
}

#define TYPE json_value_hashmap_node
#define TYPED_NAME(name) json_value_hashmap_node_##name
#define LIST_IMPLEMENTATION
#include "../../utils/list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
