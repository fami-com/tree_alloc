#include <new>

#include "header.h"
#include "mem/pimem.h"

const size_t Header::STSIZE = aligned_sizeof<Header>();

void *Header::get_body_ptr() {
    return (char *) this + Header::STSIZE;
}

Node *Header::get_node_ptr() {
    return (Node *) ((char *) this + Header::STSIZE);
}

Header *Header::from_body(void *body) {
    return (struct Header *) ((char *) body - Header::STSIZE);
}

Header *Header::get_next() {
    if (m_flags.final) {
        return nullptr;
    }

    return (Header *) ((char *) this + m_size + Header::STSIZE);
}

void *Header::get_next_uninit() {
    return (Header *) ((char *) this + m_size + Header::STSIZE);;
}

Header *Header::get_prev() {
    if (m_flags.first) {
        return nullptr;
    }

    return (Header *) ((char *) this - m_lsize - Header::STSIZE);
}

void Header::set_lsize(size_t new_lsize) {
    m_lsize = new_lsize;
}

size_t Header::get_size() const {
    return m_size;
}

void Header::set_size(size_t size) {
    m_size = size;
}

void Header::mark_free(Tree *tree) {
    if (auto sz = get_size(); sz >= Node::STSIZE) {
        insert_item(tree, init_node(get_node_ptr(), sz));
    }

    m_flags.free = true;
}

void Header::mark_reserved(Tree *tree) {
    if (auto sz = get_size(); sz >= Node::STSIZE) {
        remove_item(tree, get_node_ptr());
    }

    m_flags.free = false;
}

bool Header::is_free() const {
    return m_flags.free;
}

Header *Header::merge_right(struct Tree *tree) {
    if (auto nxt = get_next(); nxt && nxt->is_free()) {

        auto fin = nxt->is_final();
        auto fst = is_first();

        auto sz = get_size();
        auto nxt_sz = nxt->get_size();

        if (sz >= Node::STSIZE) {
            remove_item(tree, get_node_ptr());
        }

        if (nxt_sz >= Node::STSIZE) {
            remove_item(tree, nxt->get_node_ptr());
        }

        sz += nxt_sz + Header::STSIZE;
        set_size(sz);

        if (auto nxt2 = get_next()) {
            nxt2->set_lsize(sz);
        }

        if (sz >= Node::STSIZE) {
            insert_item(tree, init_node(get_node_ptr(), sz));
        }

        set_final(fin);
        set_first(fst);
    }

    return this;
}

Header *Header::merge_left(Tree *tree) {
    auto header = this;
    if (auto prev = get_prev(); prev && prev->is_free()) {
        header = prev->merge_right(tree);
    }

    return header;
}

Header::Header(size_t size, size_t lsize, bool first, bool final, Tree *tree) {
    m_size = size;
    m_lsize = lsize;
    m_flags.first = first;
    m_flags.final = final;
    mark_free(tree);
}

bool Header::split_header(size_t new_size, Tree *tree) {
    new_size = align(new_size);
    auto sz = get_size();

    auto fin = is_final();
    auto fst = is_first();

    if (sz <= new_size) {
        return false;
    }

    if (sz - new_size <= Header::STSIZE) {
        return false;
    }

    if (is_free() && sz >= Node::STSIZE) {
        remove_item(tree, get_node_ptr());
    }

    set_size(new_size);

    if (is_free()){
        mark_free(tree);
    }

    auto ptr = get_next_uninit();

    auto new_header = new(ptr) Header(sz - new_size - Header::STSIZE, new_size, false, fin, tree);

    if (auto nxt2 = new_header->get_next()) {
        nxt2->set_lsize(new_header->get_size());
    }

    new_header->mark_free(tree);
    new_header->set_final(fin);

    set_first(fst);

    return true;
}

Header *Header::from_node(Node *node) {
    return ((struct Header *)((char *)node - Header::STSIZE));
}

bool Header::is_first() const {
    return m_flags.first;
}

bool Header::is_final() const {
    return m_flags.final;
}

void Header::set_first(bool v) {
    m_flags.first = v;
}

void Header::set_final(bool v) {
    m_flags.final = v;
}


