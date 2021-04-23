#include <cstring>
#include <cstdio>
#include <cerrno>

#include <algorithm>

#include "allocator.h"
#include "arena.h"
#include "mem/pimem.h"
#include "rbtree/rbtree.h"

static Tree tree_head{.root = nullptr};

void *mem_alloc(size_t size) noexcept {
    if (size == 0) {
        return nullptr;
    }

    errno = 0;
    size = align(size);

    if (errno == ERANGE) {
        return nullptr;
    }

    Arena *arena;
    Header *header;

    auto *node = search_fitting(&tree_head, size);

    if (node) {
        header = Header::from_node(node);
    } else {
        arena = Arena::create(size, &tree_head);

        if (arena == nullptr) {
            return nullptr;
        }

        header = arena->get_first_header();
    }

    auto sz = header->get_size();

    void *ret_ptr;

    if (sz > size) {
        header->split(size, &tree_head);
    }

    header->mark_reserved(&tree_head);
    ret_ptr = header->get_body_ptr();

    return ret_ptr;
}

void mem_free(void *ptr) noexcept {
    // C11, 7.22.3.3 If ptr is a null pointer, no action occurs.
    if (ptr == nullptr) {
        return;
    }

    auto *header = Header::from_body(ptr);
    auto *arena = Arena::from_header(header);

    header->mark_free(&tree_head);

    header = header->merge_right(&tree_head);

    header = header->merge_left(&tree_head);

    if (!header->get_next() && !header->get_prev()) {
        header->mark_reserved(&tree_head);
        //printf("Delete arena with m_size %ld\n", arena->m_size);
        delete arena;
    }
}

void *mem_realloc(void *ptr, size_t new_size) noexcept {
    errno = 0;
    new_size = align(new_size);

    if (errno == ERANGE) {
        return nullptr;
    }

    if (ptr == nullptr) {
        return mem_alloc(new_size);
    }

    if (new_size == 0) {
        mem_free(ptr);
        return nullptr;
    }

    auto old_header = Header::from_body(ptr);
    auto old_size = old_header->get_size();

    if (old_size > new_size) {
        old_header->split(new_size, &tree_head);
        return ptr;
    }

    if (auto next = old_header->get_next(); next != nullptr && next->is_free() &&
                                            next->get_size() + old_size >= new_size) {
        old_header->merge_right(&tree_head);
        old_header->split(new_size, &tree_head);
        return ptr;
    }

    if (auto prev = old_header->get_prev(); prev != nullptr && prev->is_free() &&
                                            prev->get_size() + old_size >= new_size) {
        auto hdr = old_header->merge_left(&tree_head);
        hdr->split(new_size, &tree_head);

        return hdr->get_body_ptr();
    }

    void *new_ptr = mem_alloc(new_size);

    if (new_ptr == nullptr) {
        return nullptr;
    }

    memcpy(new_ptr, ptr, std::min(old_size, new_size));

    mem_free(ptr);

    return new_ptr;
}

void mem_dump() noexcept {
    puts("--- START MEMORY DUMP ---");
    printf("Page size: %zu\n", get_page_size());

    print_tree(&tree_head);
    puts("--- FINISH MEMORY DUMP ---");
}
