#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "types.h"

// todo: implement functions
// todo: defragmentation? probably comes after basic implementation (should be able to spread over several frames, ref. book)
// todo: ways of measuring fragmentation?

class HeapAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes);
    void* alloc(u64 sizeBytes);
    void free(void* buffer);
    void clear();

    // Byte usage
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // Debugging
    void printInfo() const;

private:
    struct MemoryBlock;

    MemoryBlock* m_firstBlock {};
    u64 m_maxSizeBytes {};
    u64 m_allocatedBytes {};

    MemoryBlock* getFreeBlock(u64 sizeBytes) const;
    static void merge(MemoryBlock* first, MemoryBlock* second);

    // todo: look at struct memory layout, explicitly show padding (from book)
    struct MemoryBlock
    {
        bool isFree {};
        u64 sizeBytes {};
        MemoryBlock* next {};
        MemoryBlock* previous {};
    };
};

#endif // HEAP_ALLOCATOR_H
