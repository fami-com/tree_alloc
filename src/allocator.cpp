#include <cstring>
#include <cstdio>
#include <cerrno>

#include <algorithm>

#include "allocator.h"
#include "arena.h"
#include "mem/pimem.h"
#include "rbtree/rbtree.h"

static Tree tree_head {.root = nullptr};

void *mem_alloc(size_t size) {
    if(size == 0) {
        return nullptr;
    }

    errno = 0;
    size = align(size);

    if(errno == ERANGE) {
        return nullptr;
    }

    struct Arena *arena;
    struct Header *header;

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

    if (sz == size) {
        header->mark_reserved(&tree_head);
        ret_ptr = header->get_body_ptr();
    } else if (sz > size) {
        header->split_header(size, &tree_head);

        header->mark_reserved(&tree_head);
        ret_ptr = header->get_body_ptr();
    }

    //header->mark_reserved(&tree_head);
    ret_ptr = header->get_body_ptr();

    return ret_ptr;
}

void mem_free(void *ptr) {
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

void *mem_realloc(void *ptr, size_t new_size) {
    errno = 0;
    new_size = align(new_size);

    if (errno == ERANGE){
        return nullptr;
    }

    if (ptr == nullptr) {
        return mem_alloc(new_size);
    }

    if (new_size == 0) {
        mem_free(ptr);
        return nullptr;
    }

    auto *old_header = Header::from_body(ptr);
    auto old_size = old_header->get_size();

    if (old_size > new_size) {
        old_header->split_header(new_size, &tree_head);
        return ptr;
    }

    auto new_ptr = mem_alloc(new_size);

    auto *new_header = Header::from_body(new_ptr);
    memmove(new_header->get_body_ptr(), old_header->get_body_ptr(), std::min(old_size, new_size));

    mem_free(old_header->get_body_ptr());

    return new_ptr;
}

/// deprecated
void mem_dump() {
    //char* addr = (char*)get_first_header(first_arena);
    printf("Page m_size: %zu\n", get_page_size());
    char *addr;
    int arena_num = 0;

    size_t total = 0;
    int total_count = 0;

    size_t total_total = 0;
    int total_total_count = 0;

    Arena *arena = nullptr;

    /*while (arena) {

        total = 0;
        total_count = 0;


        addr = (char *) arena->get_first_header();

        printf("Arena №%d\nUsed space: %d/%ld\n", ++arena_num, -1, arena->size);

        printf("%15s%15s%8s%15s\n", "ADDR", "FREE", "SIZE", "NEXT_ADDR");

        while (addr) {
            auto *hdr = (Header*)addr;

            printf((hdr->get_next() ? "%15p%15s%8ld%15p\n" : "%15p%15s%8ld%15s\n"),
                   addr, hdr->is_free() ? "true" : "false", hdr->get_size(),
                   (hdr->get_next() ? (char *) (hdr->get_next()) : "END OF MEMORY"));
            total += hdr->get_size();
            total_total += hdr->get_size();
            addr = (char *) hdr->get_next();
            total_count++;
            total_total_count++;
        }

        printf("Results:\n");
        printf("Total blocks allocated/used: %d\nMemory used by headers: %ld B\nMemory reserved for usage: %ld B\n",
               total_count,
               total_count * sizeof(struct Header), total);
    }*/

    if (arena_num > 1) {
        printf("\nTotal arenas allocated: %d\nTotal memory used by headers: %ld B\nTotal memory reserved for usage: %zu B\n",
               arena_num, total_total_count * sizeof(struct Header), total_total);
    }

    printf("\n");
    print_tree(&tree_head);
}
