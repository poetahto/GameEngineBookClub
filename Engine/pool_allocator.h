#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <algorithm>
#include "types.h"

template <class T>
class PoolAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes, u64 blockSizeBytes)
    {
        m_maxSizeBytes = maxSizeBytes;
        m_blockSizeBytes = std::max(blockSizeBytes, sizeof(PoolBlock));
        m_blocks = new(baseAddress) PoolBlock[getMaxBlocks()];

        // Initializes all blocks to empty states.
        clear();
    }

    // todo: alloc based alignment here, like in the book?
    void* alloc()
    {
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
        T newFreeIndex = static_cast<T>(newFreeBlock - m_blocks);
        T prevFreeIndex = m_firstFreeBlockIndex;
        m_firstFreeBlockIndex = newFreeIndex;
        newFreeBlock->nextFreeBlockIndex = prevFreeIndex;
        m_allocatedBlockCount--;
    }

    void clear()
    {
        const u64 numBlocks = getMaxBlocks();

        // Initialize our free list, so every block points to a neighbor.
        for (T i = 0; i < numBlocks - 1; ++i)
        {
            m_blocks[i].nextFreeBlockIndex = i + 1;
        }

        m_firstFreeBlockIndex = 0;
        m_allocatedBlockCount = 0;
    }

    // Byte usage
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

    // Block usage
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

    // Debugging
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
        T nextFreeBlockIndex;
    };

    PoolBlock* m_blocks{};
    T m_firstFreeBlockIndex{};
    u64 m_blockSizeBytes{};
    u64 m_maxSizeBytes{};
    u64 m_allocatedBlockCount{};
};

#endif // POOL_ALLOCATOR_H
