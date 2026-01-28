#pragma once
#include <stdlib.h>

struct Allocator_vtable {
	void* (*alloc)(void *self, size_t size);
	void (*dealloc)(void *self, void *ptr);
	void* (*realloc)(void *self, void *ptr, size_t old_size, size_t new_size);
};
typedef struct Allocator_vtable Allocator_vtable;

struct Allocator {
	void* allocator_base;
	const Allocator_vtable *vtable;
};

typedef struct Allocator Allocator;

static inline void* alloc(Allocator a, size_t size) {
	return a.vtable->alloc(a.allocator_base, size);
}

static inline void dealloc(Allocator a, void *ptr) {
	a.vtable->dealloc(a.allocator_base, ptr);
}
static inline void* arealloc(Allocator a, void *ptr, size_t old_size, size_t new_size) {
	return a.vtable->realloc(a.allocator_base, ptr, old_size, new_size);
}
