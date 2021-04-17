#ifndef ALLOC_PIMEM_H
#define ALLOC_PIMEM_H

#include <cstddef>
#include <cerrno>

void *pialloc(size_t size);

void pifree(void *ptr);

size_t get_page_size();

const size_t ALIGNMENT = alignof(max_align_t);

constexpr size_t align_to(size_t size, size_t alignment) noexcept {
    size_t ret_size = size;

    if (size_t rem = size % alignment; rem != 0) {
        ret_size += alignment - rem;
        if (ret_size < size) {
            errno = ERANGE;
            return 0;
        }
    }

    return ret_size;
}

constexpr inline size_t align(size_t size) noexcept {
    return align_to(size, ALIGNMENT);
}

template<typename T>
consteval size_t aligned_sizeof() noexcept {
    return align(sizeof(T));
}

#endif //ALLOC_PIMEM_H
