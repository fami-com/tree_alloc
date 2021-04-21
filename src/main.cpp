#include "test.h"
#include "allocator.h"
#include <cstdlib>
#include <limits>

#define N 100000U

static_assert(__cplusplus >= 202002L, "This project requires C++20 for consteval and designated initializers");

int main() {
    test(32, N);
    test(512, N);
    test(std::numeric_limits<unsigned short>::max(), N);
    return 0;
}
