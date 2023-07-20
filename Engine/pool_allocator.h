#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <algorithm>
#include "types.h"
#include "memory_util.h"

template <class TIndex>
class PoolAllocatorTemplate
{
public:
    void init(void* baseAddress, u64 maxSizeBytes, u64 blockSizeBytes, memory::Alignment align = memory::DEFAULT_ALIGNMENT)
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

    template <class TData>
    void init(void* baseAddress, u64 maxSizeBytes)
    {
        init(baseAddress, maxSizeBytes, sizeof(TData), align<TData>());
    }

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
    void printInfo() const
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
    struct PoolBlock
    {
        TIndex nextFreeBlockIndex;
    };

    PoolBlock* m_blocks{};
    TIndex m_firstFreeBlockIndex{};
    u64 m_blockSizeBytes{};
    u64 m_maxSizeBytes{};
    u64 m_allocatedBlockCount{};
};

/**
 * \brief A pool allocator that can store a huge amount of items, but wastes at most 7 bytes.
 * \details This allocator is well suited for large amounts of data.
 */
typedef PoolAllocatorTemplate<u64> PoolAllocatorLarge; // todo: replace this with a special pointer-based one for infinite items?

/**
 * \brief A pool allocator that can store 65,536 items, and only wastes at most 1 byte.
 * \details This is a good general-purpose pool allocator, as long as you don't need a
 * huge amount of items or store extremely small items.
 */
typedef PoolAllocatorTemplate<u16> PoolAllocator;

/**
 * \brief A pool allocator that can only store 256 items, but wastes no bytes.
 * \details This is a specialized pool allocator that works only if you need a very small pool,
 * storing very small items.
 */
typedef PoolAllocatorTemplate<u8> PoolAllocatorSmall;

#endif // POOL_ALLOCATOR_H
