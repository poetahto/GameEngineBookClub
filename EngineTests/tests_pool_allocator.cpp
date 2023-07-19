#include "gtest/gtest.h"
#include "pool_allocator.h"

class PoolAllocatorTest : public testing::Test
{
public:
    PoolAllocatorTest()
    {
        u64 size = 2048;
        rawBuffer = new u8[size];
        pa.init(rawBuffer, size, 1);
    }

    ~PoolAllocatorTest() override
    {
        delete rawBuffer;
    }

protected:
    PoolAllocator pa;
    u8* rawBuffer;
};

TEST_F(PoolAllocatorTest, TestGetBlocks)
{
    pa.alloc();
    EXPECT_EQ(pa.getAllocatedBlocks(), 1);
    EXPECT_EQ(pa.getMaxBlocks(), 2048 / pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks() - 1);
}

TEST_F(PoolAllocatorTest, TestGetBytes)
{
    pa.alloc();
    EXPECT_EQ(pa.getAllocatedBytes(), 1 * pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes() - pa.getBlockSizeBytes());
}

TEST_F(PoolAllocatorTest, TestClear)
{
    pa.alloc();
    pa.clear();
    EXPECT_EQ(pa.getAllocatedBytes(), 0);
    EXPECT_EQ(pa.getAllocatedBlocks(), 0);
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks());
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes());
}

TEST_F(PoolAllocatorTest, TestFree)
{
    pa.alloc();
    void* buffer = pa.alloc();
    pa.free(buffer);
    EXPECT_EQ(pa.getAllocatedBytes(), 1 * pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getAllocatedBlocks(), 1);
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks() - 1);
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes() - pa.getBlockSizeBytes());
}

TEST_F(PoolAllocatorTest, TestAlloc)
{
    pa.alloc();
    pa.alloc();
    pa.alloc();
    pa.alloc();
    pa.alloc();

    EXPECT_EQ(pa.getAllocatedBytes(), 5 * pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getAllocatedBlocks(), 5);
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks() - 5);
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes() - 5 * pa.getBlockSizeBytes());
}

TEST_F(PoolAllocatorTest, TestPlacement1)
{
    void* buffer1 = pa.alloc();
    pa.alloc();
    pa.free(buffer1);
    void* buffer2 = pa.alloc();
    EXPECT_EQ(buffer1, buffer2);
}

TEST_F(PoolAllocatorTest, TestPlacement2)
{
    pa.alloc();
    void* buffer1 = pa.alloc();
    pa.free(buffer1);
    void* buffer2 = pa.alloc();
    EXPECT_EQ(buffer1, buffer2);
}

TEST_F(PoolAllocatorTest, TestTooManyItems)
{
    for (u64 i = 0; i < pa.getMaxBlocks(); i++)
    {
        pa.alloc();
    }

    EXPECT_EQ(pa.getAllocatedBytes(), pa.getMaxSizeBytes());
    EXPECT_EQ(pa.getAllocatedBlocks(), pa.getMaxBlocks());
    EXPECT_EQ(pa.getRemainingBlocks(), 0);
    EXPECT_EQ(pa.getRemainingBytes(), 0);

    void* buffer = pa.alloc();
    EXPECT_EQ(buffer, nullptr);

    EXPECT_EQ(pa.getAllocatedBytes(), pa.getMaxSizeBytes());
    EXPECT_EQ(pa.getAllocatedBlocks(), pa.getMaxBlocks());
    EXPECT_EQ(pa.getRemainingBlocks(), 0);
    EXPECT_EQ(pa.getRemainingBytes(), 0);

    pa.clear();

    EXPECT_EQ(pa.getAllocatedBytes(), 0);
    EXPECT_EQ(pa.getAllocatedBlocks(), 0);
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks());
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes());

    pa.alloc();

    EXPECT_EQ(pa.getAllocatedBlocks(), 1);
    EXPECT_EQ(pa.getMaxBlocks(), 2048 / pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getRemainingBlocks(), pa.getMaxBlocks() - 1);
    EXPECT_EQ(pa.getAllocatedBytes(), 1 * pa.getBlockSizeBytes());
    EXPECT_EQ(pa.getRemainingBytes(), pa.getMaxSizeBytes() - pa.getBlockSizeBytes());
}
