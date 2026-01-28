#include "hashmap.h"
#include "hashmap_node.h"
#include "../string/string_view.h"

static const size_t HASHMAP_INITIAL_SIZE = 7;
static const size_t HASH_PRIME = 53;
static const double MAX_LOAD_FACTOR = 3;
static const double MIN_LOAD_FACTOR = MAX_LOAD_FACTOR / 4;

static size_t string_hash(string_view str, size_t size) {
	// Implements a simple polynomial rolling hash function
	// https://cp-algorithms.com/string/string-hashing.html
	size_t hash = 0;
	size_t pow = 1;
	for (size_t i = 0; i < str.size; i++) {
		hash += (sv_at_unchecked(str, i) * pow);
		hash %= size;
		pow *= HASH_PRIME;
	}
	return hash;
}

static double get_load_factor(const json_value_hashmap *map) {
	return (double)map->size / (double)map->buckets.length;
}

void json_value_hashmap_init(json_value_hashmap *map, Allocator a) {
	json_value_hashmap_node_list_init(&map->buckets, HASHMAP_INITIAL_SIZE, a);
	memset(map->buckets.data, 0,
		   sizeof(json_value_hashmap_node) * map->buckets.capacity);
	map->buckets.length = HASHMAP_INITIAL_SIZE;
	map->size = 0;
}

Result json_value_hashmap_get(const json_value_hashmap *map, string_view key,
							  JSONValue *out) {
	size_t node_index = string_hash(key, map->buckets.length);
	json_value_hashmap_node node;
	node = json_value_hashmap_node_list_get_unchecked(&map->buckets, node_index);
	if (node.key.arr.data == NULL) {
		Result r = new_errorf("Key not found in hashmap: \"%.*s\"",
			EHashmapKeyNotFound,
			(int)key.size, key.data);
		return r;
	}
	return json_value_hashmap_node_get(&node, key, out);
}

static void json_value_hashmap_set_internal(json_value_hashmap *map, string key,
                                            JSONValue value, bool rehash,
                                            Allocator a, Allocator stra);

static void rehash(json_value_hashmap *map, size_t new_bucket_count, Allocator a, Allocator stra) {
	size_t old_bucket_count = map->buckets.length;
	json_value_hashmap_node_list old_buckets = map->buckets;

	json_value_hashmap_node_list_init(&map->buckets, new_bucket_count, a);
	memset(map->buckets.data, 0, 
		   sizeof(json_value_hashmap_node) * new_bucket_count);
	map->buckets.length = new_bucket_count;
	for (size_t bucket = 0; bucket < old_bucket_count; bucket++) {
		json_value_hashmap_node node;
		node = json_value_hashmap_node_list_get_unchecked(&old_buckets, bucket);
		if (node.key.arr.data == NULL) {
			continue;
		}
		json_value_hashmap_set_internal(map, node.key, node.value, false, a, stra);
		json_value_hashmap_node *curr = node.next;
		while (curr != NULL) {
			json_value_hashmap_set_internal(map, curr->key, curr->value, false, a, stra);
			json_value_hashmap_node *next = curr->next;
			dealloc(a, curr);
			curr = next;
		}
	}
	json_value_hashmap_node_list_free(&old_buckets, a);
}

void json_value_hashmap_set(json_value_hashmap *map, string key,
							JSONValue value, Allocator a) {
	json_value_hashmap_set_internal(map, key, value, true, a, a);
}

void json_value_hashmap_set_split(json_value_hashmap *map, string key,
							JSONValue value, Allocator a, Allocator stra) {
	json_value_hashmap_set_internal(map, key, value, true, a, stra);
}

static void json_value_hashmap_set_internal(json_value_hashmap *map, string key,
								 JSONValue value, bool should_rehash, Allocator a, Allocator stra) {
	double load_factor = get_load_factor(map);
	if (should_rehash && load_factor > MAX_LOAD_FACTOR) {
		rehash(map, map->buckets.length * 2, a, stra);
	} else if (should_rehash && load_factor < MIN_LOAD_FACTOR &&
			   map->buckets.length > HASHMAP_INITIAL_SIZE) {
		rehash(map, map->buckets.length / 2, a, stra);
	}
	size_t node_index = string_hash(as_sv(key), map->buckets.length);
	json_value_hashmap_node *node;
	node = json_value_hashmap_node_list_get_ref_unchecked(&map->buckets, node_index);
	if (node->key.arr.data == NULL) {
		// Create new node
		node->key = key;
		node->value = value;
		node->next = NULL;
		map->size += 1;
	} else {
		// Update existing node
		if (json_value_hashmap_node_set_split(node, key, value, a, stra)) {
			// New entry added instead of overwritten
			map->size += 1;
		}
	}
}

void json_value_hashmap_free(json_value_hashmap *map, Allocator a) {
	json_value_hashmap_free_split(map, a, a);
}

void json_value_hashmap_free_split(json_value_hashmap *map, Allocator a, Allocator stra) {
	for (size_t i = 0; i < map->buckets.length; i++) {
		json_value_hashmap_node node;
		node = json_value_hashmap_node_list_get_unchecked(&map->buckets, i);
		if (node.key.arr.data == NULL) {
			continue;
		}
		json_value_hashmap_node_free_split(&node, a, stra);
	}
	json_value_hashmap_node_list_free(&map->buckets, a);
}

#define TYPE json_value_hashmap_node
#define TYPED_NAME(name) json_value_hashmap_node_##name
#define LIST_IMPLEMENTATION
#include "../list.h"
#undef LIST_IMPLEMENTATION
#undef TYPE
#undef TYPED_NAME
