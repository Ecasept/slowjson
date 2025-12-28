#ifndef TYPE
#error "You need to define `TYPE` to create a generic array!"
#endif

#ifndef TYPED_NAME
#error "You need to define `TYPED_NAME` to create a generic array!"
#endif

#include "custom_error.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct TYPED_NAME(list) {
	TYPE *data;
	size_t length;
	size_t capacity;
	bool freed;
};

typedef struct TYPED_NAME(list) TYPED_NAME(list);

// ===== Forward declarations =====
void TYPED_NAME(list_init)(TYPED_NAME(list) * arr, size_t cap);
void TYPED_NAME(list_resize)(TYPED_NAME(list) * arr, size_t new_capacity);
void TYPED_NAME(list_ensure_resize)(TYPED_NAME(list) * arr, size_t capacity);
void TYPED_NAME(list_push)(TYPED_NAME(list) * arr, TYPE element);
void TYPED_NAME(list_get)(const TYPED_NAME(list) * arr, size_t index,
						  TYPE *element);
Result TYPED_NAME(list_get_err)(const TYPED_NAME(list) * arr, size_t index,
								TYPE *element);
void TYPED_NAME(list_ensure_index)(const TYPED_NAME(list) * arr, size_t index);
void TYPED_NAME(list_get_ref)(const TYPED_NAME(list) * arr, size_t index,

							  TYPE **element);
void TYPED_NAME(list_set)(TYPED_NAME(list) * arr, size_t index, TYPE element);
void TYPED_NAME(list_extend)(TYPED_NAME(list) * arr,
							 const TYPED_NAME(list) * other);
void TYPED_NAME(list_free)(TYPED_NAME(list) * arr);

void TYPED_NAME(list_ensure_not_freed)(const TYPED_NAME(list) * arr);

/**
 * @brief Returns whether `index` points to a valid location in `arr`
 */
static inline bool TYPED_NAME(list_check_index)(const TYPED_NAME(list) * arr,
												size_t index) {
	return index < arr->length;
}

#ifdef LIST_IMPLEMENTATION

const size_t INITIAL_LIST_SIZE = 8;

/** @brief Panics if the passed list is freed */
void TYPED_NAME(list_ensure_not_freed)(const TYPED_NAME(list) * arr) {
	if (arr->freed) {
		panic("Array has already been freed");
	}
}

/**
 * @brief Initializes a new array at the given location
 *
 * @param arr Pointer to the array that should be initialized
 * @param cap The initial capacity of the array. Defaults to `INITIAL_LIST_SIZE`
 * if `0` is passed.
 */
void TYPED_NAME(list_init)(TYPED_NAME(list) * arr, size_t cap) {
	if (cap == 0) {
		cap = INITIAL_LIST_SIZE;
	}
	arr->data = (TYPE *)malloc(sizeof(TYPE) * cap);
	if (arr->data == NULL) {
		panic("Failed to allocate memory for array");
	}
	arr->capacity = cap;
	arr->length = 0;
	arr->freed = false;
}

/**
 * @brief Resizes `arr` so that it has a capacity of `new_capacity`
 *
 * Returns an error if `new_capacity` is smaller than the current size
 */
void TYPED_NAME(list_resize)(TYPED_NAME(list) * arr, size_t new_capacity) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	if (new_capacity < arr->length) {
		panic("Resized size is smaller than current size of array");
	}

	// Resize to a power of two (more than needed) to reduce resizing operations
	size_t new_cap = arr->capacity;
	while (new_cap < new_capacity) {
		new_cap *= 2;
	}

	TYPE *new_data = (TYPE *)realloc(arr->data, new_cap * sizeof(TYPE));
	if (new_data == NULL) {
		panic("Failed to allocate memory");
	}
	arr->data = new_data;
	arr->capacity = new_cap;
}

/**
 * @brief Ensures that `arr` has a capacity of `capacity`.
 *
 * If not, it resizes the array.
 */
void TYPED_NAME(list_ensure_resize)(TYPED_NAME(list) * arr, size_t capacity) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	if (arr->capacity < capacity) {
		TYPED_NAME(list_resize)(arr, capacity);
	}
}

/**
 * @brief Appends a copy of `element` to the array
 */
void TYPED_NAME(list_push)(TYPED_NAME(list) * arr, TYPE element) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	TYPED_NAME(list_ensure_resize)(arr, arr->length + 1);
	arr->data[arr->length] = element;
	arr->length += 1;
}

/**
 * @brief Writes a copy of the element at `index` to the location of `element`
 */
void TYPED_NAME(list_get)(const TYPED_NAME(list) * arr, size_t index,
						  TYPE *element) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	TYPED_NAME(list_ensure_index)(arr, index);
	*element = arr->data[index];
}

/**
 * @brief Writes a copy of the element at `index` to the location of `element`.
 * Returns an `EIndexOutOfBounds` exception if the index is not covered by the
 * array instead of panicking.
 */
Result TYPED_NAME(list_get_err)(const TYPED_NAME(list) * arr, size_t index,
								TYPE *element) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	if (!TYPED_NAME(list_check_index)(arr, index)) {
		return new_errorf("Index %ld out of bounds for array of length %ld",
						  EIndexOutOfBounds, index, arr->length);
	}
	*element = arr->data[index];
	return new_success();
}

/**
 * @brief Returns an error if the index lies outside the length of the array
 */
void TYPED_NAME(list_ensure_index)(const TYPED_NAME(list) * arr, size_t index) {
	if (!TYPED_NAME(list_check_index)(arr, index)) {
		panicf("Tried to access array with length %ld at index %ld",
			   arr->length, index);
	}
}

/**
 * @brief Writes a reference to the element at `index` to the location of
 * `element`
 */
void TYPED_NAME(list_get_ref)(const TYPED_NAME(list) * arr, size_t index,
							  TYPE **element) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	TYPED_NAME(list_ensure_index)(arr, index);
	*element = &(arr->data[index]);
}

/**
 * @brief Set the element of `arr` at `index` to a copy of `element`
 */
void TYPED_NAME(list_set)(TYPED_NAME(list) * arr, size_t index, TYPE element) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	TYPED_NAME(list_ensure_index)(arr, index);
	arr->data[index] = element;
}

/**
 * @brief Appends all of the elements from `other` to `arr`
 *
 * @param other The array to copy other elements from
 */
void TYPED_NAME(list_extend)(TYPED_NAME(list) * arr,
							 const TYPED_NAME(list) * other) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	TYPED_NAME(list_ensure_not_freed)(other);

	size_t old_len = arr->length;
	size_t new_len = arr->length + other->length;
	TYPED_NAME(list_ensure_resize)(arr, new_len);

	memcpy(&arr->data[old_len], other->data, other->length * sizeof(TYPE));
	arr->length = new_len;
}

void TYPED_NAME(list_free)(TYPED_NAME(list) * arr) {
	TYPED_NAME(list_ensure_not_freed)(arr);
	free(arr->data);
	arr->freed = true;
}
#endif
