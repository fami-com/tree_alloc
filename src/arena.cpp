#include <new>

#include "arena.h"
#include "mem/pimem.h"

const size_t Arena::STSIZE = aligned_sizeof<Arena>();

struct Arena *Arena::create(size_t size, struct Tree *tree) {
    if (size == 0) {
        size = 1;
    }

    errno = 0;

    size = align_to(size + Arena::STSIZE + Header::STSIZE, get_page_size());

    if(errno == ERANGE){
        return nullptr;
    }

    auto data = pialloc(size);

    if (data == nullptr) {
        return nullptr;
    }

    auto ptr = new(data) Arena(size - Arena::STSIZE);

    new(ptr->get_first_header()) Header(ptr->size - Header::STSIZE, 0, true, tree);

    return ptr;
}

void Arena::operator delete(void *ptr) noexcept {
    pifree(ptr);
}

Header *Arena::get_first_header() {
    return (struct Header*)((char *)this + Arena::STSIZE);
}

Arena *Arena::from_header(Header *header) {
    while(header->get_prev() != nullptr) {
        header = header->get_prev();
    }

    auto a = (Arena *) ((char *) header - Arena::STSIZE);
    return a;
}

Arena::Arena(size_t size) {
    this->size = size;
}
