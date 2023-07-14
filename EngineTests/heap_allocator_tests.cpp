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
    auto pointer = heap.alloc(15);
    EXPECT_GT(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getMaxSizeBytes() - heap.getAllocatedBytes(), heap.getRemainingBytes());
    heap.free(pointer);
    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
    heap.alloc(20);
    heap.alloc(20);
    heap.alloc(20);
    heap.alloc(20);
    EXPECT_GT(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getMaxSizeBytes() - heap.getAllocatedBytes(), heap.getRemainingBytes());
    heap.clear();
    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
}

TEST_F(HeapAllocatorTest, DataIntegrity)
{
    auto ptr1 = heap.alloc(sizeof(u16));
    new (ptr1->address) u16 { 5 };
    auto ptr2 = heap.alloc(sizeof(u16));
    new (ptr2->address) u16 { 24 };
    auto ptr3 = heap.alloc(sizeof(u16));
    new (ptr3->address) u16 { 78 };
    heap.printInfo();

    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->address), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr2->address), 24);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->address), 78);

    heap.free(ptr2);
    heap.printInfo();

    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->address), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->address), 78);

    auto ptr4 = heap.alloc(sizeof(u16));
    new (ptr4->address) u16 { 23 };
    heap.printInfo();

    EXPECT_EQ(*reinterpret_cast<u16*>(ptr1->address), 5);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr3->address), 78);
    EXPECT_EQ(*reinterpret_cast<u16*>(ptr4->address), 23);

    heap.clear();

    EXPECT_EQ(heap.getAllocatedBytes(), 0);
    EXPECT_EQ(heap.getRemainingBytes(), heap.getMaxSizeBytes());
}
