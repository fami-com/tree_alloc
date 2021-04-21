#include <algorithm>
#include <random>
#include <functional>

#include <cstring>

#include "test.h"
#include "allocator.h"

using std::min;

static std::random_device rd;
static std::mt19937 mt;

void fill_rand(void *ptr, size_t size) noexcept {
    auto cptr = static_cast<char *>(ptr);
    for (size_t i = 0; i < size; i++) {
        *cptr++ = static_cast<char>(mt());
    }
}

unsigned int get_checksum(void *ptr, size_t size) noexcept {
    unsigned sum = 0;
    for (size_t i = 0; i < size; i++) {
        char v = *(static_cast<char *>(ptr) + i);
        // boost hash function
        sum ^= std::hash<char>()(v) + 0x9e3779b9 + (sum << 6) + (sum >> 2);
    }
    return sum;
}

void test(size_t max_size, unsigned N) noexcept {
    unsigned seed = rd();
    mt.seed(seed);
    OperationResult results[ARRAY_SIZE] = {{.addr = nullptr, .size = 0, .checksum = 0}};
    printf("Starting test with max size %ld, seed %u:\n", max_size, seed);

    for (unsigned i = 0; i < N; i++) {
        // 0 - ALLOC
        // 1 - REALLOC
        // 2 - FREE
        unsigned char action = mt() % 3;
        size_t size = mt() % max_size + 1;
        unsigned rand_index = mt() % ARRAY_SIZE;
        OperationResult result;

        if (i % N / 100 == 0) {
            printf("%02u%%\r", i);
            fflush(stdout);
        }

        switch (action) {
            case 0: { // ALLOC
                void *ptr = mem_alloc(size);
                if (ptr) {
                    fill_rand(ptr, size);
                    result = {
                            .addr = ptr,
                            .size = size,
                            .checksum = get_checksum(ptr, size),
                    };

                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                    mem_free(results[rand_index].addr);
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
                void *ptr = mem_realloc(result.addr, size);

                if (ptr != nullptr) {
                    assert(get_checksum(ptr, min(size, result.size)) == controll && "bad checksum");
                    fill_rand(ptr, size);

                    results[rand_index] = {
                            .addr = ptr,
                            .size = size,
                            .checksum = get_checksum(ptr, size),
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
                        .addr = nullptr,
                        .size = 0,
                        .checksum = 0,
                };

                break;
            }
            default:
                break;
        }
    }

    for (unsigned i = 0; i < ARRAY_SIZE; i++) {
        assert(get_checksum(results[i].addr, results[i].size) == results[i].checksum && "bad checksum");
        mem_free(results[i].addr);
    }

    printf("Test with maximum size %ld finished successfully\n", max_size);
}
