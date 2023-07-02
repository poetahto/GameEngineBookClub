#include <gtest/gtest.h>
#include "memory_manager.h"

class StackAllocatorTest : public ::testing::Test {
public:
    StackAllocatorTest()
    {
        size_t size = 2048;
        rawBuffer = new char[size];
        sa.init(rawBuffer, size);
    }
    ~StackAllocatorTest()
    {
        delete rawBuffer;
    }

protected:
    StackAllocator sa;
    char* rawBuffer;
};

// Adapted from https://stackoverflow.com/questions/42093360/how-to-check-if-a-pointer-points-to-a-properly-aligned-memory-location
inline bool is_aligned(const void* pointer, size_t align)
{
    uintptr_t pointerAddr = reinterpret_cast<uintptr_t>(pointer);
    return !(pointerAddr % align);
}

void TestCorrectAlignment(StackAllocator& sa, size_t align)
{
    sa.alloc(align / 2); // Purposely set ourselves up for a misalignment.
    void* buffer = sa.alloc(64, Align(align));
    EXPECT_TRUE(is_aligned(buffer, align));
}

void TestIncorrectAlignment(StackAllocator& sa, size_t align)
{
    sa.alloc(align / 2); // Purposely set ourselves up for a misalignment.
    void* buffer = sa.alloc(64);
    EXPECT_FALSE(is_aligned(buffer, align));
}

TEST_F(StackAllocatorTest, CorrectAlignment)
{
    TestCorrectAlignment(sa, 2);
    TestCorrectAlignment(sa, 4);
    TestCorrectAlignment(sa, 8);
    TestCorrectAlignment(sa, 16);
    TestCorrectAlignment(sa, 128);
}

// Kinda meta-testing to make sure our alignment testing method works.
TEST_F(StackAllocatorTest, IncorrectAlignment)
{
    TestIncorrectAlignment(sa, 2);
    TestIncorrectAlignment(sa, 4);
    TestIncorrectAlignment(sa, 8);
    TestIncorrectAlignment(sa, 16);
    TestIncorrectAlignment(sa, 128);
}

TEST_F(StackAllocatorTest, MarkerResetButNoChange)
{
    size_t size = sa.getMaxSize();
    sa.alloc(size);
    auto marker = sa.getMarker();
    sa.freeToMarker(marker);

    // If we get a marker and immidiately reset to it, our 
    // size should not change.
    EXPECT_EQ(sa.getAllocatedBytes(), size);
}

TEST_F(StackAllocatorTest, MarkerResetWithChange)
{
    sa.alloc(100);
    EXPECT_EQ(sa.getAllocatedBytes(), 100);
    auto marker = sa.getMarker();
    void* bufferA = sa.alloc(100);

    // Cumulative allocations should add up.
    EXPECT_EQ(sa.getAllocatedBytes(), 200);
    sa.freeToMarker(marker);

    // Freeing via marker should rollback the allocated bytes.
    EXPECT_EQ(sa.getAllocatedBytes(), 100);
    void* bufferB = sa.alloc(200);

    // We should be able to allocate directly into that newly freed memory.
    EXPECT_EQ(bufferA, bufferB);
}

TEST_F(StackAllocatorTest, CantFitTooBig)
{
    void* buffer = sa.alloc(sa.getMaxSize() + 1);

    // We shouldn't get a valid result from a failed allocation.
    EXPECT_EQ(buffer, nullptr);

    // Nothing should change when we can't fit an item.
    EXPECT_EQ(sa.getAllocatedBytes(), 0);
    EXPECT_EQ(sa.getRemainingBytes(), sa.getMaxSize());
}

TEST_F(StackAllocatorTest, CantAddWhenFull)
{
    // Try to add something that would overflow the stack.
    sa.alloc(sa.getMaxSize());
    void* buffer = sa.alloc(1);

    // We shouldn't get anything valid back.
    EXPECT_EQ(buffer, nullptr);

    // Our size shouldn't change.
    EXPECT_EQ(sa.getAllocatedBytes(), sa.getMaxSize());
    EXPECT_EQ(sa.getRemainingBytes(), 0);
}

TEST_F(StackAllocatorTest, BarelyEnoughRoom)
{
    // Try to add something that exactly meeting our capacity.
    void* buffer = sa.alloc(sa.getMaxSize());

    // We should get something valid back.
    EXPECT_NE(buffer, nullptr);

    // We shouldn't have any room at all left.
    EXPECT_EQ(sa.getRemainingBytes(), 0);
}

TEST_F(StackAllocatorTest, AbleToMakeRoom)
{
    // Fill up all space.
    sa.alloc(sa.getMaxSize());

    // Empty stack and try to fill it again.
    sa.clear();
    void* buffer = sa.alloc(sa.getMaxSize());

    // We should get something valid back.
    EXPECT_NE(buffer, nullptr);
}

TEST_F(StackAllocatorTest, ResetToZero)
{
    void* bufferA = sa.alloc(16);
    void* bufferB = sa.alloc(16);
    sa.clear();

    // We should have nothing left after clearing.
    EXPECT_EQ(sa.getAllocatedBytes(), 0);
    EXPECT_EQ(sa.getRemainingBytes(), sa.getMaxSize());

    // Both initial buffers should have been stored in the same memory location.
    void* bufferC = sa.alloc(16);
    void* bufferD = sa.alloc(16);
    EXPECT_EQ(bufferA, bufferC);
    EXPECT_EQ(bufferD, bufferB);
}
