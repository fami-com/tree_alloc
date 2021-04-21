#ifndef ALLOC_ALLOCATOR_H
#define ALLOC_ALLOCATOR_H

#include <stddef.h>

void *mem_alloc(size_t size) noexcept;

void mem_free(void *ptr) noexcept;

void *mem_realloc(void *ptr, size_t new_size) noexcept;

void mem_dump() noexcept;

#endif //ALLOC_ALLOCATOR_H


