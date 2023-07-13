#include <gtest/gtest.h>
#include "heap_allocator.h"

class HeapAllocatorTest : public testing::Test
{
public:
    HeapAllocatorTest()
    {
        u64 size = 2048;
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

TEST_F(HeapAllocatorTest, Test)
{
    HeapPointer* b1 = heap.alloc(5);
    HeapPointer* b2 = heap.alloc(15);
    HeapPointer* b3 = heap.alloc(8);
    HeapPointer* b4 = heap.alloc(32);
    heap.free(b1);
    heap.free(b3);
    heap.printInfo();

    heap.defragment();
    heap.printInfo();

    heap.defragment();
    heap.printInfo();
}
