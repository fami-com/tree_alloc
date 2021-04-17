#include "test.h"
#define min(a, b) a < b ? a : b

void random_addr(void *ptr, size_t size) {
    for (size_t i = 0;
         i < size;
         i++) {
        *((char *) ptr + i) = (char) rand();
    }
}

unsigned int get_checksum(void *ptr, size_t size) {
    unsigned int sum = 0;
    for (size_t i = 0;
         i < size;
         i++)
        // include position of each bit
        sum = (sum << 2) ^ (sum >> 5) ^ *((char *) ptr + i);
    return sum;
}

void test(size_t max_size, int N) {
    unsigned int seed = time(nullptr);
    void *ptr1;
    unsigned controll;
    srand(seed);
    struct OperationResult results[ARRAY_SIZE] = {{.addr = nullptr, .size = 0, .checksum = 0}};
    printf("Starting test with max m_size %ld, \nseed %u:\n", max_size, seed);

    for (unsigned int i = 0;
         i < N;
         i++) {
        // 0 - ALLOC
        // 1 - REALLOC
        // 2 - FREE
        unsigned short action = rand() % 3;
        size_t size = rand() % max_size;
        unsigned int rand_index = rand() % ARRAY_SIZE;
        struct OperationResult result;
        void *ptr;
        if (i % N / 100 == 0) {
            printf("%u\n", i);
            fflush (stdout);
        }

        switch (action) {
            case 0: // ALLOC
                ptr = mem_alloc(size);
                if (ptr) {
                    random_addr(ptr, size);
                    result = (struct OperationResult) {
                            .addr = ptr,
                            .size = size,
                            .checksum = get_checksum(ptr, size),
                    };
                    mem_free(results[rand_index].addr);
                    results[rand_index] = result;
                }
                break;
            case 1:
                result = results[rand_index];
                if (result.addr) { // Якщо був алоцирований
                    if (get_checksum(result.addr, result.size) != result.checksum) {
                        printf("Current: %u\nExprected: %u", get_checksum(result.addr, result.size), result.checksum);
                    }
                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                }
                controll = get_checksum(result.addr, min(size, result.size));
                ptr1 = mem_realloc(result.addr, size);
                if (ptr1) {
                    if (get_checksum(ptr1, min(size, result.size)) != controll) {
                        printf("Current: %u\nExprected: %u\nNew m_size: %ld\nOld Size: %ld\n",
                               get_checksum(ptr1, min(size, result.size)),
                               controll,
                               size,
                               result.size);
                    }
                    assert(get_checksum(ptr1, min(size, result.size)) == controll && "bad checksum");
                    random_addr(ptr1, size);
                    results[rand_index] = (struct OperationResult) {
                            .addr = ptr1,
                            .size = size,
                            .checksum = get_checksum(ptr1, size),
                    };
                }
                break;
            case 2:
                result = results[rand_index];
                if (result.addr) { // Якщо був алоцирований
                    if (get_checksum(result.addr, result.size) != result.checksum) {
                        printf("Current: %u\nExprected: %u",
                               get_checksum(result.addr, result.size),
                               result.checksum);
                    }
                    assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
                }
                mem_free(result.addr);
                results[rand_index] = (struct OperationResult) {
                        .addr = nullptr,
                        .size = 0,
                        .checksum = 0,
                };

                break;
            default:
                break;
        }
    }

    for (auto & result : results) {
        assert(get_checksum(result.addr, result.size) == result.checksum && "bad checksum");
        mem_free(result.addr);
    }

    printf("Test with maximum m_size %ld finished successfully\n", max_size);
}