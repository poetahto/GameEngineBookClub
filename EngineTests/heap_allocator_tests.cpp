#include <gtest/gtest.h>
#include "heap_allocator.h"

class HeapAllocatorTest : public testing::Test
{
public:
    HeapAllocatorTest()
    {
        u64 size = 2048;
        rawBuffer = new u8[size];
        ha.init(rawBuffer, size);
    }

    ~HeapAllocatorTest() override
    {
        delete rawBuffer;
    }

protected:
    HeapAllocator ha;
    u8* rawBuffer;
};

TEST_F(HeapAllocatorTest, Test)
{
    auto b1 = ha.alloc(5);
    auto b2 = ha.alloc(15);
    auto b3 = ha.alloc(8);
    ha.free(b1);
    ha.free(b3);
    ha.free(b2);

    ha.printInfo();
}
