#ifndef ALLOC_ARENA_H
#define ALLOC_ARENA_H

#include <cstddef>
#include "header.h"

struct Arena {
    size_t size;

    static const size_t STSIZE;

    static Arena *from_header(Header *header);

    static Arena* create(size_t size, struct Tree *tree);

    void operator delete(void* ptr) noexcept;

    Header *get_first_header();

private:
    explicit Arena(size_t size);
};

#endif //ALLOC_ARENA_H
