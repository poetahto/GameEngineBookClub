#include "gtest/gtest.h"
#include "pool_allocator.h"

class PoolAllocatorTest : public testing::Test
{
public:
    PoolAllocatorTest()
    {
        constexpr u64 size = 2048;
        rawBuffer = new u8[size];
        pa.init(rawBuffer, size);
    }

    ~PoolAllocatorTest()
    {
        delete rawBuffer;
    }
protected:
    PoolAllocator<1> pa;
    u8* rawBuffer;
};

TEST_F(PoolAllocatorTest, test)
{
    // test code here
    pa.printInfo();
}
