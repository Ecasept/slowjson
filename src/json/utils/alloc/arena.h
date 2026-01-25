#pragma once
#include "allocator.h"

struct Arena {
	unsigned char *start;
	unsigned char *chunk;
	size_t offset;
	size_t current_chunk_size;
};
typedef struct Arena Arena;

Arena new_arena();
Allocator arena_as_allocator(Arena *a);
void arena_free(Arena *a);
