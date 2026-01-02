#include "hashmap.h"
#include "hashmap_node.h"

const static size_t HASHMAP_INITIAL_SIZE = 7;
const static size_t HASH_PRIME = 53;
const static double MAX_LOAD_FACTOR = 3;
const static double MIN_LOAD_FACTOR = MAX_LOAD_FACTOR / 4;

static size_t string_hash(const string *str, size_t size) {
	// Implements a simple polynomial rolling hash function
	// https://cp-algorithms.com/string/string-hashing.html
	size_t hash = 0;
	size_t pow = 1;
	for (size_t i = 0; i < str->arr.length; i++) {
		uchar v;
		string_at(str, i, &v);
		hash += (v * pow);
		hash %= size;
		pow *= HASH_PRIME;
	}
	return hash;
}

static double get_load_factor(const json_value_hashmap *map) {
	return (double)map->size / (double)map->buckets.length;
}

void json_value_hashmap_init(json_value_hashmap *map) {
	json_value_hashmap_node_list_init(&map->buckets, HASHMAP_INITIAL_SIZE);
	memset(map->buckets.data, 0,
		   sizeof(json_value_hashmap_node) * map->buckets.capacity);
	map->buckets.length = HASHMAP_INITIAL_SIZE;
	map->size = 0;
}

Result json_value_hashmap_get_cstr(const json_value_hashmap *map, const char *key_cstr,
									   JSONValue *out) {
	string key;
	string_new(&key, key_cstr);
	Result r = json_value_hashmap_get(map, key, out);
	string_free(&key);
	return r;
}

Result json_value_hashmap_get(const json_value_hashmap *map, const string key,
							  JSONValue *out) {
	size_t node_index = string_hash(&key, map->buckets.length);
	json_value_hashmap_node node;
	json_value_hashmap_node_list_get(&map->buckets, node_index, &node);
	if (node.key.arr.data == NULL) {
		return new_errorf("Key not found", EHashmapKeyNotFound);
	}
	return json_value_hashmap_node_get(&node, key, out);
}

static void json_value_hashmap_set_internal(json_value_hashmap *map, string key,
									  JSONValue value, bool rehash);

static void rehash(json_value_hashmap *map, size_t new_bucket_count) {
	size_t old_bucket_count = map->buckets.length;
	json_value_hashmap_node_list old_buckets = map->buckets;

	json_value_hashmap_node_list_init(&map->buckets, new_bucket_count);
	memset(map->buckets.data, 0, 
		   sizeof(json_value_hashmap_node) * new_bucket_count);
	map->buckets.length = new_bucket_count;
	for (size_t bucket = 0; bucket < old_bucket_count; bucket++) {
		json_value_hashmap_node node;
		json_value_hashmap_node_list_get(&old_buckets, bucket, &node);
		while (node.key.arr.data != NULL) {
			json_value_hashmap_set_internal(map, node.key, node.value, false);
			node = *node.next;
		}
	}
	json_value_hashmap_node_list_free(&old_buckets);
}

void json_value_hashmap_set(json_value_hashmap *map, string key,
							JSONValue value) {
	json_value_hashmap_set_internal(map, key, value, true);
}

static void json_value_hashmap_set_internal(json_value_hashmap *map, string key,
									  JSONValue value, bool should_rehash) {
	double load_factor = get_load_factor(map);
	if (should_rehash && load_factor > MAX_LOAD_FACTOR) {
		rehash(map, map->buckets.length * 2);
	} else if (should_rehash && load_factor < MIN_LOAD_FACTOR &&
			   map->buckets.length > HASHMAP_INITIAL_SIZE) {
		rehash(map, map->buckets.length / 2);
	}
	size_t node_index = string_hash(&key, map->buckets.length);
	json_value_hashmap_node *node;
	json_value_hashmap_node_list_get_ref(&map->buckets, node_index, &node);
	if (node->key.arr.data == NULL) {
		// Create new node
		node->key = key;
		node->value = value;
		node->next = NULL;
		map->size += 1;
	} else {
		// Update existing node
		if (json_value_hashmap_node_set(node, key, value)) {
			// New entry added instead of overwritten
			map->size += 1;
		}
	}
}

void json_value_hashmap_free(json_value_hashmap *map) {
	for (size_t i = 0; i < map->buckets.length; i++) {
		json_value_hashmap_node node;
		json_value_hashmap_node_list_get(&map->buckets, i, &node);
		if (node.key.arr.data == NULL) {
			continue;
		}
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
