#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <algorithm>
#include <cassert>
#include "..\types.hpp"
#include "memory_util.hpp"

template <class TIndex>
class IndexedPoolAllocator
{
public:
    /**
     * \brief Creates a new instance of a stack allocator.
     * \param baseAddress The starting point for storing allocated data.
     * \param maxSizeBytes The upper-limit for allocations.
     * \param blockSizeBytes The size of each block that will store user data.
     * \param align The alignment requirements for each block of data.
     */
    void init(
        void* baseAddress,
        u64 maxSizeBytes,
        u64 blockSizeBytes,
        memory::Alignment align = memory::DEFAULT_ALIGNMENT)
    {
        // Align the base pointer, so that each subsequent element maintains this alignment,
        u8* alignedBaseAddress = memory::alignPointer(static_cast<u8*>(baseAddress), align);
        u64 shift = alignedBaseAddress - static_cast<u8*>(baseAddress);

        m_maxSizeBytes = maxSizeBytes - shift;
        m_blockSizeBytes = std::max(blockSizeBytes, sizeof(PoolBlock));
        m_blocks = new (baseAddress) PoolBlock[getMaxBlocks()];

        // Initializes all blocks to empty states.
        clear();
    }

    /**
     * \brief Creates a new instance of a stack allocator, with block size and
     * alignment tuned for storing a certain data type.
     * \tparam TData The type of data this allocator will store.
     * \param baseAddress The starting point for storing allocated data.
     * \param maxSizeBytes The upper-limit for allocations.
     */
    template <class TData>
    void init(void* baseAddress, u64 maxSizeBytes)
    {
        init(baseAddress, maxSizeBytes, sizeof(TData), align<TData>());
    }

    /**
     * \brief Reserves a block of memory for general use.
     * \return A pointer to the memory that has been reserved.
     * \details This operation runs in O(1) time.
     */
    void* alloc()
    {
        // We can't allocate a block if we're out of blocks.
        if (getRemainingBlocks() <= 0)
        {
            // todo: log out of memory
            return nullptr;
        }

        PoolBlock* result = m_blocks + m_firstFreeBlockIndex;
        m_firstFreeBlockIndex = result->nextFreeBlockIndex;
        m_allocatedBlockCount++;
        return result;
    }

    /**
     * \brief Releases a reserved block of memory, so that it can be reused later.
     * \param buffer The block of memory that should be freed.
     * \details This operation runs in O(1) time.
     */
    void free(void* buffer)
    {
        assert(buffer != nullptr);
        PoolBlock* newFreeBlock = static_cast<PoolBlock*>(buffer);
        TIndex newFreeIndex = static_cast<TIndex>(newFreeBlock - m_blocks);
        TIndex prevFreeIndex = m_firstFreeBlockIndex;
        m_firstFreeBlockIndex = newFreeIndex;
        newFreeBlock->nextFreeBlockIndex = prevFreeIndex;
        m_allocatedBlockCount--;
    }

    /**
     * \brief Releases all blocks of reserved memory.
     * \details This operation runs in O(n) time, where n == total # of blocks.
     */
    void clear()
    {
        const u64 numBlocks = getMaxBlocks();

        // Initialize our free list, so every block points to a neighbor.
        for (TIndex i = 0; i < numBlocks - 1; ++i)
        {
            m_blocks[i].nextFreeBlockIndex = i + 1;
        }

        m_firstFreeBlockIndex = 0;
        m_allocatedBlockCount = 0;
    }

    // === Byte Usage ===
    u64 getMaxSizeBytes() const
    {
        return m_maxSizeBytes;
    }

    u64 getRemainingBytes() const
    {
        return getMaxSizeBytes() - getAllocatedBytes();
    }

    u64 getAllocatedBytes() const
    {
        return getAllocatedBlocks() * getBlockSizeBytes();
    }

    // === Block Usage ===
    u64 getMaxBlocks() const
    {
        return getMaxSizeBytes() / getBlockSizeBytes();
    }

    u64 getRemainingBlocks() const
    {
        return getMaxBlocks() - getAllocatedBlocks();
    }

    u64 getAllocatedBlocks() const
    {
        return m_allocatedBlockCount;
    }

    // === Debugging ===
    void printInfo() const // todo: logger / better output
    {
        printf("=== POOL ALLOCATOR ===\n");
        printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());
        printf("blocks: [%llu/%llu] %llu free\n", getAllocatedBlocks(), getMaxBlocks(), getRemainingBlocks());
        printf("block size: [%llu bytes] nextBlock=%llu\n", getBlockSizeBytes(), sizeof(void*));
        printf("======================\n");
    }

    u64 getBlockSizeBytes() const
    {
        return m_blockSizeBytes;
    }

private:
    // An unused block of memory in the allocator.
    struct PoolBlock
    {
        TIndex nextFreeBlockIndex;
    };

    // Pointer to the base address for storing blocks.
    PoolBlock* m_blocks{};

    TIndex m_firstFreeBlockIndex{};
    u64 m_blockSizeBytes{};
    u64 m_maxSizeBytes{};
    u64 m_allocatedBlockCount{};
};

/**
 * \brief A pool allocator that can store a huge amount of items, but wastes at most 7 bytes.
 * \details This allocator is well suited for large amounts of items.
 */
typedef IndexedPoolAllocator<u64> PoolAllocatorLarge; // todo: replace this with a special pointer-based one for infinite items?

/**
 * \brief A pool allocator that can store 65,536 items, and only wastes at most 1 byte.
 * \details This is a good general-purpose pool allocator, as long as you don't need a
 * huge amount of items, or can't afford a small amount of items.
 */
typedef IndexedPoolAllocator<u16> PoolAllocator;

/**
 * \brief A pool allocator that can only store 256 items, but wastes no bytes.
 * \details This is a specialized pool allocator that works only if you
 * can afford a very small number of total items.
 */
typedef IndexedPoolAllocator<u8> PoolAllocatorSmall;

#endif // POOL_ALLOCATOR_H
