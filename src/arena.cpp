#include <new>

#include "arena.h"
#include "mem/pimem.h"

const size_t Arena::STSIZE = aligned_sizeof<Arena>();

struct Arena *Arena::create(size_t size, struct Tree *tree) {
    if (size == 0) {
        size = 1;
    }

    size = align_to(size + Arena::STSIZE, get_page_size());

    if(errno == ERANGE){
        return nullptr;
    }

    auto data = pialloc(size);

    if (!data) {
        return nullptr;
    }

    auto ptr = new(data) Arena(size - Arena::STSIZE);

    new(ptr->get_first_header()) Header(ptr->size - Header::STSIZE, 0, true, true, tree);

    return ptr;
}

void Arena::operator delete(void *ptr) noexcept {
    pifree(ptr);
}

Header *Arena::get_first_header() {
    return (struct Header*)((char *)this + Arena::STSIZE);
}

Arena *Arena::from_header(Header *header) {
    for (; header->get_prev(); header = header->get_prev());

    return (struct Arena *) ((char *) header - Arena::STSIZE);
}

Arena::Arena(size_t size) {
    this->size = size;
}
