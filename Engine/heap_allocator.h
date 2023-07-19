#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "memory_util.h"
#include "pool_allocator.h"
#include "types.h"

// todo: add debug option of clearing empty bits to an easy-to-read value (this applies to all allocators)

/**
 * \brief A pointer to some memory being managed by a heap.
 * \details You should avoid saving references to the raw pointer inside,
 * as it may be relocated and result in undefined behavior.
 */
struct HeapPointer
{
    u8* rawPtr;
};

/**
 * \brief A generic allocator that can reserve, free, and defragment memory.
 * \details This allocator can only reserve in multiples of 16-bytes, due to the book-keeping data that must be stored.
 * 8 bytes are unavoidably allocated for each block of reserved memory, to keep track of the block's length.
 */
class HeapAllocator
{
    /**
     * \brief A block of memory managed by the heap.
     * \details If the block is free, then the "next" pointer points to the next available free block.
     * Otherwise, the "next" pointer is overwritten with user data.
     */
    struct MemoryBlock
    {
        u64 sizeBytes;
        MemoryBlock* next;
    };

public:
    /**
     * \brief Creates a new instance of a heap allocator.
     * \param baseAddress The starting point for storing allocated data.
     * \param maxSizeBytes The upper-limit for allocations.
     */
    void init(void* baseAddress, u64 maxSizeBytes);

    /**
     * \brief Allocates room on the heap for storing generic data.
     * \param sizeBytes The amount of user-data required.
     * \param alignment How much the user-data should be aligned.
     * \return A reserved location on the heap. Access this memory via "HeapPointer->rawPtr",
     * as it may be relocated during defragmentation.
     * \details This operation runs in O(n) time, but approaches O(1) with no fragmentation.
     */
    HeapPointer* alloc(u64 sizeBytes, memory::Alignment alignment = memory::DEFAULT_ALIGNMENT);

    /**
     * \brief Frees an allocated block from the heap.
     * \param pointer The memory that should be freed.
     * \details This operation runs in O(n) time, but approaches O(1) with no fragmentation.
     */
    void free(HeapPointer* pointer);

    /**
     * \brief Frees all allocated blocks on the heap.
     */
    void clear();

    /**
     * \brief Attempts to group free blocks at the end of the heap.
     * \details This operation runs in O(1) time, and will require at most N iterations
     * to fully defragment the heap, where N is the number of allocated blocks.
     */
    void defragment();

    /**
     * \brief Checks to see if the heap contains any fragmented (potentially wasted) blocks.
     * \return True if the heap is fragmented, false otherwise.
     * \details This operation runs in O(1) time, is essentially free.
     */
    bool isFragmented() const;

    // === Byte usage ===
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // === Debugging ===
    void printInfo() const;

private:
    // the buffer for storing relocatable pointers
    PoolAllocatorTemplate<u64> m_pointers{};

    MemoryBlock* m_freeBlocks{};
    u64 m_freeBlockCount{};
    u8* m_baseAddress{};
    u64 m_maxSizeBytes{};
    u64 m_maxAllocations{};
    u64 m_allocatedBytes{};

    void merge(MemoryBlock* first, MemoryBlock* second);
    static bool areAdjacent(MemoryBlock* first, MemoryBlock* second);
};

#endif // HEAP_ALLOCATOR_H
