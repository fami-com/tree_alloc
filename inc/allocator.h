#ifndef ALLOC_ALLOCATOR_H
#define ALLOC_ALLOCATOR_H

#include <cstddef>

void *mem_alloc(size_t size);

void mem_free(void *ptr);

void *mem_realloc(void *ptr, size_t new_size);

void mem_dump();

#endif //ALLOC_ALLOCATOR_H


