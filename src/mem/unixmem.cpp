#include <sys/mman.h>
#include <unistd.h>

#include "mem/pimem.h"
#include "arena.h"

static size_t page_size;

void *pialloc(size_t size) {
    auto ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if(ptr == MAP_FAILED){
        return nullptr;
    }

    return ptr;
}

void pifree(void *ptr) {
    munmap(ptr, static_cast<Arena*>(ptr)->size);
}

size_t get_page_size() {
    if (!page_size) {
        page_size = size_t(sysconf(_SC_PAGESIZE));
    }
    return page_size;
}
