#include <algorithm>
#include <random>
#include <functional>

#include <cstring>

#include "test.h"
#include "allocator.h"

//#undef assert
//#define assert(a) nullptr
//#undef printf
//#define printf(fmt, ...) nullptr

using std::min;

std::random_device rd;
std::mt19937 mt;

void fill_rand(void *ptr, size_t size) {
    auto cptr = (char*)ptr;
    for (size_t i = 0; i < size; i++) {
        *cptr++ = (char) mt();
    }
}

unsigned int get_checksum(void *ptr, size_t size) {
    unsigned sum = 0;
    for (size_t i = 0; i < size; i++) {
        char v = *((char *) ptr + i);
        // boost hash function
        sum ^= std::hash<char>()(v) + 0x9e3779b9 + (sum<<6) + (sum>>2);
    }
    return sum;
}

void test(size_t max_size, int N) {
    unsigned seed = rd();
    mt.seed(seed);
    OperationResult results[ARRAY_SIZE] = {{.addr = NULL, .size = 0, .checksum = 0}};
    printf("Starting test with max size %ld, seed %u:\n", max_size, seed);

    for (unsigned int i = 0; i < N; i++) {
        // 0 - ALLOC
        // 1 - REALLOC
        // 2 - FREE
        unsigned char action = mt() % 3;
        size_t size = mt() % max_size + 1;
        unsigned rand_index = mt() % ARRAY_SIZE;
        OperationResult result;
        void *ptr;

        if (i % N / 100 == 0) {
            printf("%u%%\r", i);
            fflush (stdout);
        }

        switch (action) {
            case 0: { // ALLOC
                ptr = mem_alloc(size);
                if (ptr) {
                    fill_rand(ptr, size);
                    result = {
                            .addr = ptr,
                            .size = size,
                            .checksum = get_checksum(ptr, size),
                    };

                    mem_free(results[rand_index].addr);
                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                    results[rand_index] = result;
                }
                break;
            }
            case 1: {
                result = results[rand_index];

                if (result.addr != nullptr) {
                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                }
                unsigned controll = get_checksum(result.addr, min(size, result.size));
                void *ptr1 = mem_realloc(result.addr, size);

                if (ptr1 != nullptr) {
                    assert(get_checksum(ptr1, min(size, result.size)) == controll && "bad checksum");
                    fill_rand(ptr1, size);

                    results[rand_index] = {
                            .addr = ptr1,
                            .size = size,
                            .checksum = get_checksum(ptr1, size),
                    };
                }
                break;
            }
            case 2: {
                result = results[rand_index];

                if (result.addr != nullptr) {
                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                }

                mem_free(result.addr);
                results[rand_index] = {
                        .addr = NULL,
                        .size = 0,
                        .checksum = 0,
                };

                break;
            }
            default:
                break;
        }
    }

    for (unsigned i = 0;i < ARRAY_SIZE;i++) {
        assert(get_checksum(results[i].addr, results[i].size) == results[i].checksum && "bad checksum");
        mem_free(results[i].addr);
    }

    printf("Test with maximum size %ld finished successfully\n", max_size);
}