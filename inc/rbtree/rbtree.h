#ifndef ALLOC_RBTREE_H
#define ALLOC_RBTREE_H

#include <cstddef>

extern const size_t NODE_SIZE;

enum Color {
    BLACK,
    RED
};

struct Node {
    Color color;

    Node *parent;
    Node *left;
    Node *right;
    Node *next;
    Node *prev;
    size_t value;

    static const size_t STSIZE;
};

static Node NODENIL {
        .color = BLACK,
        .parent = &NODENIL,
        .left = &NODENIL,
        .right = &NODENIL,
        .next = nullptr,
        .prev = nullptr,
        .value = 0,
};

struct Tree {
    Node *root;
};

Node *init_node(Node *node_place, size_t value) noexcept;

void insert_item(Tree *tree, Node *z) noexcept;

void remove_item(Tree *tree, Node *node) noexcept;

Node *search_fitting(struct Tree *tree, size_t value) noexcept;

Node *search(struct Tree *tree, size_t value) noexcept;

void print_tree(struct Tree *tree) noexcept;

void print_node(struct Node *node) noexcept;

#endif
