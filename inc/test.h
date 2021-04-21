#ifndef ALLOC_TEST_H
#define ALLOC_TEST_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define ARRAY_SIZE 500
extern "C" {

struct OperationResult {
    void *addr;
    size_t size;
    unsigned checksum;
};

void test(size_t max_size, unsigned N) noexcept;

}
#endif //ALLOC_TEST_H
