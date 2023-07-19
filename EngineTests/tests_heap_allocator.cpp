#include <gtest/gtest.h>
#include "heap_allocator.h"

class HeapAllocatorTest : public testing::Test
{
public:
    HeapAllocatorTest()
    {
        u64 size = 10000;
        rawBuffer = new u8[size];
        heap.init(rawBuffer, size);
    }

    ~HeapAllocatorTest() override
    {
        delete rawBuffer;
    }

protected:
    HeapAllocator heap;
    u8* rawBuffer;
};

TEST_F(HeapAllocatorTest, ByteUsage)
{
    EXPECT_FALSE(heap.isFragmented());
    auto pointer = heap.alloc(15);
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_GT(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getMaxSizeBytes() - heap.getAllocatedBytes(), heap.getRemainingBytes());
    heap.free(pointer);
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
    heap.alloc(20);
    heap.alloc(20);
    heap.alloc(20);
    heap.alloc(20);
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_GT(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getMaxSizeBytes() - heap.getAllocatedBytes(), heap.getRemainingBytes());
    heap.clear();
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
}

TEST_F(HeapAllocatorTest, DataIntegrity)
{
    auto ptr1 = heap.alloc(sizeof(u16), Align(2));
    new (ptr1->rawPtr) u16 { 5 };
    auto ptr2 = heap.alloc(sizeof(u16), Align(2));
    new (ptr2->rawPtr) u16 { 24 };
    auto ptr3 = heap.alloc(sizeof(u16), Align(2));
    new (ptr3->rawPtr) u16 { 78 };

    EXPECT_TRUE(isAligned(ptr1, 2));
    EXPECT_TRUE(isAligned(ptr2, 2));
    EXPECT_TRUE(isAligned(ptr3, 2));
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->rawPtr), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr2->rawPtr), 24);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->rawPtr), 78);
    EXPECT_FALSE(heap.isFragmented());

    heap.free(ptr2);

    EXPECT_TRUE(heap.isFragmented());
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->rawPtr), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->rawPtr), 78);
    EXPECT_TRUE(isAligned(ptr1, 2));
    EXPECT_TRUE(isAligned(ptr3, 2));

    auto ptr4 = heap.alloc(sizeof(u16), Align(2));
    new (ptr4->rawPtr) u16 { 23 };

    EXPECT_FALSE(heap.isFragmented());
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->rawPtr), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->rawPtr), 78);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr4->rawPtr), 23);
    EXPECT_TRUE(isAligned(ptr1, 2));
    EXPECT_TRUE(isAligned(ptr3, 2));
    EXPECT_TRUE(isAligned(ptr4, 2));

    heap.clear();

    EXPECT_FALSE(heap.isFragmented());
    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
}

TEST_F (HeapAllocatorTest, SmallTuck)
{
    auto ptr1 = heap.alloc(sizeof(u8), Align(1));
    new (ptr1->rawPtr) u8 { 5 };
    auto ptr2 = heap.alloc(sizeof(u8), Align(1));
    new (ptr2->rawPtr) u8 { 14 };
    auto ptr3 = heap.alloc(sizeof(u8), Align(1));
    new (ptr3->rawPtr) u8 { 3 };
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_TRUE(isAligned(ptr1, 1));
    EXPECT_TRUE(isAligned(ptr2, 1));
    EXPECT_TRUE(isAligned(ptr3, 1));

    auto oldPtr2Raw = ptr2->rawPtr;
    heap.free(ptr2);
    EXPECT_TRUE(heap.isFragmented());
    EXPECT_TRUE(isAligned(ptr1, 1));
    EXPECT_TRUE(isAligned(ptr3, 1));

    auto ptr4 = heap.alloc(25);
    EXPECT_NE(oldPtr2Raw, ptr4->rawPtr);
    EXPECT_EQ(*ptr1->rawPtr, 5);
    EXPECT_EQ(*ptr3->rawPtr, 3);
    EXPECT_TRUE(heap.isFragmented());
    EXPECT_TRUE(isAligned(ptr1, 1));
    EXPECT_TRUE(isAligned(ptr3, 1));
    EXPECT_TRUE(isAligned(ptr4, 8));

    auto ptr5 = heap.alloc(sizeof(u8), Align(1));
    new (ptr5->rawPtr) u8 { 122 };
    EXPECT_EQ(ptr5->rawPtr, oldPtr2Raw);
    EXPECT_EQ(*ptr1->rawPtr, 5);
    EXPECT_EQ(*ptr3->rawPtr, 3);
    EXPECT_EQ(*ptr5->rawPtr, 122);
    EXPECT_EQ(*oldPtr2Raw, 122);
    EXPECT_FALSE(heap.isFragmented());
    EXPECT_TRUE(isAligned(ptr1, 1));
    EXPECT_TRUE(isAligned(ptr3, 1));
    EXPECT_TRUE(isAligned(ptr4, 8));
    EXPECT_TRUE(isAligned(ptr5, 1));
}
