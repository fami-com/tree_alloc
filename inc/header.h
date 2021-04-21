#ifndef ALLOC_HEADER_H
#define ALLOC_HEADER_H

#include <cstdint>
#include <cstddef>
#include "rbtree/rbtree.h"


class Header {
    size_t m_size;
    size_t m_lsize;

    struct Flags {
        bool free: 1;
        bool final: 1;
    } m_flags;

public:
    static const size_t STSIZE;

    Header(size_t size, size_t lsize, bool final, Tree *tree) noexcept;

    void *get_body_ptr() noexcept;

    Node *get_node_ptr() noexcept;

    static Header *from_body(void *body) noexcept;

    static Header *from_node(Node *node) noexcept;

    [[nodiscard]] size_t get_size() const noexcept;

    Header *get_prev() noexcept;

    Header *get_next() noexcept;

    void *get_next_uninit() noexcept;

    [[nodiscard]] bool is_free() const noexcept;

    [[nodiscard]] bool is_first() const noexcept;

    [[nodiscard]] bool is_final() const noexcept;

    Header *merge_right(Tree *tree) noexcept;

    Header *merge_left(Tree *tree) noexcept;

    bool split(size_t new_size, Tree *tree) noexcept;

    void mark_free(Tree *tree) noexcept;

    void mark_reserved(Tree *tree) noexcept;

    void set_final(bool v) noexcept;

    void set_size(size_t new_size) noexcept;

    void set_lsize(size_t new_lsize) noexcept;
};

#endif //ALLOC_HEADER_H
