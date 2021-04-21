#include <new>
#include <algorithm>

#include "arena.h"
#include "mem/pimem.h"

#define DEFAULT_ARENA_PAGE_AMNT 16

const size_t Arena::STSIZE = aligned_sizeof<Arena>();

struct Arena *Arena::create(size_t size, struct Tree *tree) {
    if (size == 0) {
        size = 1;
    }

    auto sum = size + Arena::STSIZE + Header::STSIZE;

    if (sum < size){
        return nullptr;
    }

    errno = 0;

    size = align_to(sum, get_page_size());

    if(errno == ERANGE){
        return nullptr;
    }

    size = std::max(size, DEFAULT_ARENA_PAGE_AMNT * get_page_size());

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
    return reinterpret_cast<Header*>(reinterpret_cast<char*>(this) + Arena::STSIZE);
}

Arena *Arena::from_header(Header *header) {
    while(header->get_prev() != nullptr) {
        header = header->get_prev();
    }

    auto a = reinterpret_cast<Arena*>(reinterpret_cast<char*>(header) - Arena::STSIZE);
    return a;
}

Arena::Arena(size_t new_size) {
    size = new_size;
}
