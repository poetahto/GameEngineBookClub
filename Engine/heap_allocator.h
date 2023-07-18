#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "types.h"
#include "pool_allocator.h"

// todo: add debug option of clearing empty bits to an easy-to-read value (this applies to all allocators)
// todo: look at struct memory layout, explicitly show padding (from book)

/*
 * alloc() - O(n), constant if no fragmentation
 * free() - O(n), constant if no fragmentation
 * defragment() - O(1)
 *
 * Bookkeeping overhead / restrictions:
 * +16 bytes allocated for every free and used block (8 for size, 8 for header)
 * blocks can only be allocated in 8-byte chunks (so we always have room for free-block metadata)
 */

struct MemoryBlock
{
    struct
    {
        u64 sizeBytes;
    } header {};

    struct
    {
        MemoryBlock* next;
    } freeData {};
};

struct HeapPointer
{
    u8* rawPtr;
};

class HeapAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes);
    HeapPointer* alloc(u64 sizeBytes);
    void free(HeapPointer* pointer);
    void clear();
    void defragment();

    // Byte usage
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;
    bool isFragmented() const;

    // Debugging
    void printInfo() const;

private:
    // the buffer for storing relocatable pointers
    PoolAllocator<sizeof(HeapPointer)> m_pointers {};

    MemoryBlock* m_freeBlocks {};
    u64 m_freeBlockCount {};
    u8* m_baseAddress {};
    u64 m_maxSizeBytes {};
    u64 m_maxAllocations {};
    u64 m_allocatedBytes {};

    void merge(MemoryBlock* first, MemoryBlock* second);
    static bool areAdjacent(MemoryBlock* first, MemoryBlock* second);
};

#endif // HEAP_ALLOCATOR_H
