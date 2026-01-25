#pragma once
#include "allocator.h"

static inline void * ga_alloc(void *self, size_t size) {
	(void)self;
	return malloc(size);
}

static inline void ga_dealloc(void *self, void *ptr) {
	(void)self;
	free(ptr);
}

static inline void * ga_realloc(void *self, void *ptr, size_t old_size, size_t new_size) {
	(void)self;
	(void)old_size;
	return realloc(ptr, new_size);
}

static const Allocator_vtable ga_vtable = {
    .alloc = &ga_alloc,
    .dealloc = &ga_dealloc,
    .realloc = &ga_realloc
};

static const Allocator ga = {
    .allocator_base = NULL,
    .vtable = &ga_vtable
};
