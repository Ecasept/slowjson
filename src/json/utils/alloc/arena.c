#include "arena.h"
#include <stdlib.h>
#include "mmap.h"
#include <stddef.h>
#include "../custom_error.h"
#include <string.h>

#ifdef VALGRIND_ENABLED
#include <valgrind/memcheck.h>
#endif
#ifdef ASAN_ENABLED
#include <sanitizer/asan_interface.h>
#endif

static const size_t PAGE_SIZE = 4096;
static const size_t ALIGNMENT_BYTES = _Alignof(max_align_t);

struct chunk_metadata {
	size_t chunk_size;
	void *next_chunk;
};
typedef struct chunk_metadata chunk_metadata;

#define align_to(size, alignment) \
(((size) + ((alignment)-1)) & ~((alignment)-1))

static inline chunk_metadata *get_metadata(void *chunk) {
	return (chunk_metadata *)chunk;
}

static const size_t ALIGNED_METADATA_SIZE = align_to(sizeof(chunk_metadata), ALIGNMENT_BYTES);

/**
 * @brief Allocates a new chunk to the arena that is at least as big as `size`.
 */
static inline void arena_new_chunk(Arena *arena, size_t size) {
	size = align_to(size + ALIGNED_METADATA_SIZE, PAGE_SIZE);
	void *new_chunk = map_memory(size);
	if (check_mmap_error(new_chunk)) {
		panicf("Failed to allocate memory for arena: %s", get_mem_err());
	}
	if (arena->chunk != NULL) {
		chunk_metadata *old_chunk_metadata = get_metadata(arena->chunk);
		old_chunk_metadata->next_chunk = new_chunk;
	}

	chunk_metadata *new_chunk_metadata = get_metadata(new_chunk);
	new_chunk_metadata->chunk_size = size;
	new_chunk_metadata->next_chunk = NULL;

	arena->chunk = new_chunk;
	arena->offset = ALIGNED_METADATA_SIZE;
	arena->current_chunk_size = size;

#ifdef VALGRIND_ENABLED
	VALGRIND_MAKE_MEM_NOACCESS(arena.chunk, size);
#endif
#ifdef ASAN_ENABLED
	__asan_poison_memory_region(arena.chunk, size);
#endif
}

static inline void* arena_alloc(Arena *a, size_t size) {
	if (size == 0) {
		return NULL;
	}
	size = align_to(size, ALIGNMENT_BYTES);
	if (a->offset + size > a->current_chunk_size) {
		arena_new_chunk(a, size);
		// Will allocate at least enough space for `size`
	}
	void *ptr = (void *)(a->chunk + a->offset);
	a->offset += size;

#ifdef VALGRIND_ENABLED
	VALGRIND_MAKE_MEM_UNDEFINED(ptr, size);
#endif
#ifdef ASAN_ENABLED
	__asan_unpoison_memory_region(ptr, size);
#endif

	return ptr;
}

static inline void arena_dealloc(Arena *a, void *ptr) {
	// No-op
	(void)a;
	(void)ptr;
}

void arena_free(Arena *a) {
	void *chunk = a->start;
	while (chunk != NULL) {
		chunk_metadata *metadata = get_metadata(chunk);
		void *next_chunk = metadata->next_chunk;
		if (check_unmap_error(unmap_memory(chunk, metadata->chunk_size))) {
			panicf("Failed to free arena memory: %s", get_mem_err());
		}
		chunk = next_chunk;

		#ifdef VALGRIND_ENABLED
				VALGRIND_MAKE_MEM_NOACCESS(a->base, ARENA_BASE_CHUNK_SIZE);
		#endif
		#ifdef ASAN_ENABLED
				__asan_poison_memory_region(a->base, ARENA_BASE_CHUNK_SIZE);
		#endif
	}
}

static inline void *arena_realloc(Arena *a, void* ptr, size_t old_size, size_t new_size) {
	if (ptr == NULL) {
		return arena_alloc(a, new_size);
	}
	if (new_size == 0) {
		return NULL;
	}
	void* new_ptr = arena_alloc(a, new_size);
	size_t to_copy = old_size < new_size ? old_size : new_size;
	memmove(new_ptr, ptr, to_copy);
	return new_ptr;
}

void *arena_alloc_wrapper(void *a, size_t size) {
	return arena_alloc((Arena *)a, size);
}
void arena_dealloc_wrapper(void *a, void *ptr) {
	arena_dealloc((Arena *)a, ptr);
}
void *arena_realloc_wrapper(void *a, void *ptr, size_t old_size, size_t new_size) {
	return arena_realloc((Arena *)a, ptr, old_size, new_size);
}

Arena new_arena(void) {
	Arena arena = {0};
	arena_new_chunk(&arena, PAGE_SIZE);
	arena.start = arena.chunk;
	return arena;
}

static const Allocator_vtable arena_vtable = {
	.alloc = arena_alloc_wrapper,
	.dealloc = arena_dealloc_wrapper,
	.realloc = arena_realloc_wrapper
};

Allocator arena_as_allocator(Arena *arena_ptr) {
	Allocator a = {
		.allocator_base = arena_ptr,
		.vtable = &arena_vtable
	};
	return a;
}
