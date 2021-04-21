#include <new>
#include <cstdio>

#include "header.h"
#include "mem/pimem.h"

const size_t Header::STSIZE = aligned_sizeof<Header>();

void *Header::get_body_ptr() {
    return reinterpret_cast<char*>(this) + Header::STSIZE;
}

Node *Header::get_node_ptr() {
    return reinterpret_cast<Node*>(reinterpret_cast<char*>(this) + Header::STSIZE);
}

Header *Header::from_body(void *body) {
    return reinterpret_cast<Header*>(reinterpret_cast<char*>(body) - Header::STSIZE);
}

Header *Header::get_next() {
    if (m_flags.final) {
        return nullptr;
    }

    return reinterpret_cast<Header*>(reinterpret_cast<char*>(this) + m_size + Header::STSIZE);
}

void *Header::get_next_uninit() {
    return reinterpret_cast<Header*>(reinterpret_cast<char*>(this) + m_size + Header::STSIZE);
}

Header *Header::get_prev() {
    if (m_lsize == 0) {
        return nullptr;
    }

    return reinterpret_cast<Header*>(reinterpret_cast<char*>(this) - m_lsize - Header::STSIZE);
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
    if (auto onxt = get_next(); onxt != nullptr && onxt->is_free()) {
        auto fin = onxt->is_final();
        auto fre = is_free();

        if (fre && get_size() >= Node::STSIZE) {
            remove_item(tree, get_node_ptr());
        }

        if (onxt->get_size() >= Node::STSIZE) {
            remove_item(tree, onxt->get_node_ptr());
        }

        set_size(get_size() + onxt->get_size() + Header::STSIZE);

        set_final(fin);

        if (auto nxt = get_next(); nxt != nullptr) {
            nxt->set_lsize(get_size());
        }

        if (fre && get_size() >= Node::STSIZE) {
            mark_free(tree);
            //insert_item(tree, init_node(get_node_ptr(), get_size()));
        }
    }

    return this;
}

Header *Header::merge_left(Tree *tree) {
    auto header = this;
    if (auto prev = get_prev(); prev != nullptr && prev->is_free()) {
        header = prev->merge_right(tree);
    }

    return header;
}

Header::Header(size_t size, size_t lsize, bool final, Tree *tree) {
    m_size = size;
    m_lsize = lsize;
    m_flags.final = final;
    mark_free(tree);
}

bool Header::split(size_t new_size, Tree *tree) {
    new_size = align(new_size);
    auto sz = get_size();

    auto next_size = sz - new_size - Header::STSIZE;
    if(next_size > sz){
        next_size = 0;
    }

    auto fin = is_final();

    if (sz <= new_size) {
        return false;
    }

    if (sz - new_size <= Header::STSIZE) {
        return false;
    }

    if (is_free() && sz >= Node::STSIZE) {
        auto t = get_node_ptr();
        remove_item(tree, t);
    }

    set_size(new_size);

    if (is_free()){
        mark_free(tree);
    }

    auto ptr = get_next_uninit();

    auto new_header = new(ptr) Header(next_size, new_size, fin, tree);

    set_final(false);

    if (auto nxt2 = new_header->get_next(); nxt2 != nullptr) {
        nxt2->set_lsize(new_header->get_size());
    }

    //new_header->mark_free(tree);

    return true;
}

Header *Header::from_node(Node *node) {
    return reinterpret_cast<Header*>(reinterpret_cast<char*>(node) - Header::STSIZE);
}

bool Header::is_first() const {
    return m_lsize == 0;
}

bool Header::is_final() const {
    return m_flags.final;
}

void Header::set_final(bool v) {
    m_flags.final = v;
}


