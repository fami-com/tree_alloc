#ifndef ALLOC_TEST_H
#define ALLOC_TEST_H

#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <ctime>

#include "allocator.h"

#define ARRAY_SIZE 500

struct OperationResult {
    void *addr;
    size_t size;
    unsigned int checksum;
};

void test(size_t max_size, int N);

#endif //ALLOC_TEST_H
