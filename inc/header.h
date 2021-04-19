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

    Header(size_t size, size_t lsize, bool final, Tree *tree);

    void *get_body_ptr();

    Node *get_node_ptr();

    static Header *from_body(void *body);

    static Header *from_node(Node *node);

    [[nodiscard]] size_t get_size() const;

    Header *get_prev();

    Header *get_next();

    void *get_next_uninit();

    [[nodiscard]] bool is_free() const;

    [[nodiscard]] bool is_first() const;

    [[nodiscard]] bool is_final() const;

    Header *merge_right(Tree* tree);

    Header *merge_left(Tree* tree);

    bool split_header(size_t new_size, Tree* tree);

    void mark_free(Tree *tree);

    void mark_reserved(Tree *tree);

    void set_final(bool v);

    void set_size(size_t new_size);

    void set_lsize(size_t new_lsize);
};

#endif //ALLOC_HEADER_H