#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "types.h"
#include "pool_allocator.h"

// todo: ways of measuring fragmentation?
// todo: add debug option of clearing empty bits to an easy-to-read value (this applies to all allocators)
// todo: look at struct memory layout, explicitly show padding (from book)

struct MemoryBlock
{
    // Header
    struct
    {
        u64 sizeBytes{};
    } header;

    union
    {
        u8* data {};
        MemoryBlock* next;
    };
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

    // Debugging
    void printInfo() const;

private:
    // the buffer for storing relocatable pointers
    PoolAllocator<sizeof(HeapPointer)> m_pointers {};

    MemoryBlock* m_freeBlocks {};
    u8* m_baseAddress {};
    u64 m_maxAllocations {};
    u64 m_maxBlockSize {};
    u64 m_maxSizeBytes {};
    u64 m_allocatedBytes {};

    static void merge(MemoryBlock* first, MemoryBlock* second);
};

#endif // HEAP_ALLOCATOR_H
