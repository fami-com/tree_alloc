#include "test.h"
#include "allocator.h"
#include <cstdlib>
#include <limits>
#define N 100000

int main() {
    test(32, N);
    test(512, N);
    test(std::numeric_limits<short>::max(), N);
    return 0;
}