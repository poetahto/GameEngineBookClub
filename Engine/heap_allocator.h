#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "types.h"
#include "pool_allocator.h"

// todo: ways of measuring fragmentation?

class HeapPointer
{
public:
    explicit HeapPointer(void* basePointer);
    void* getPointer() const;

private:
    void* m_basePointer;

    friend class HeapAllocator;
};

class HeapAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes);
    HeapPointer* alloc(u64 sizeBytes);
    void free(HeapPointer* heapPointer);
    void clear();
    void defragment();

    // Byte usage
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // Debugging
    void printInfo() const;

private:
    struct MemoryBlock;

    PoolAllocator<sizeof(HeapPointer)> m_heapPointers{};
    MemoryBlock* m_firstBlock{};
    HeapPointer* m_firstHeapPointer{};
    u64 m_maxSizeBytes{};
    u64 m_allocatedBytes{};

    MemoryBlock* getFreeBlock(u64 sizeBytes = 0) const;
    static void merge(MemoryBlock* first, MemoryBlock* second);

    // todo: look at struct memory layout, explicitly show padding (from book)
    struct MemoryBlock
    {
        bool isFree{};
        u64 sizeBytes{};
        MemoryBlock* next{};
        MemoryBlock* previous{};
    };
};

#endif // HEAP_ALLOCATOR_H
