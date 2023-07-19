#include <gtest/gtest.h>

#include "heap_allocator.h"

int main(int argc, char** argv)
{
    printf("%llu\n", sizeof(MemoryBlock));
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
