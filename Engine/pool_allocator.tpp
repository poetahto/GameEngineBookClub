#include <cassert>
#include "pool_allocator.h"
#include <cstdio>

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::init(void* baseAddress, u64 maxSizeBytes)
{
    m_maxSizeBytes = maxSizeBytes;
    m_blocks = new (baseAddress) PoolBlock[getMaxBlocks()];

    // Initializes all blocks to empty states.
    clear();
};

template<u64 BlockSizeBytes>
void* PoolAllocator<BlockSizeBytes>::alloc()
{
    if (m_firstFreeBlock == nullptr)
    {
        // todo: log that we are out of memory
        return nullptr;
    }

    void* result = m_firstFreeBlock->data;
    m_firstFreeBlock = m_firstFreeBlock->nextFreeBlock;
    return result;
};

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::free(void* buffer)
{
    assert(buffer != nullptr);
    PoolBlock* prevFreeBlock = m_firstFreeBlock;
    m_firstFreeBlock = static_cast<PoolBlock*>(buffer);
    m_firstFreeBlock->nextFreeBlock = prevFreeBlock;
}

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::clear()
{
    u64 numBlocks = getMaxBlocks();

    // Initialize our free list, so every block points to a neighbor.
    // I think we have to do this for each clear, since we don't track
    // free blocks.
    for (int i = 0; i < numBlocks - 1; i++)
    {
        m_blocks[i].nextFreeBlock = &m_blocks[i + 1];
    }

    // The final block has no neighbor.
    m_blocks[numBlocks - 1].nextFreeBlock = nullptr;

    m_firstFreeBlock = &m_blocks[0];
}

// === BYTE USAGE ===

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getMaxSizeBytes()
{
    return m_maxSizeBytes;
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getRemainingBytes()
{
    return getMaxSizeBytes() - getAllocatedBytes();
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getAllocatedBytes()
{
    return getAllocatedBlocks() * BlockSizeBytes;
}

// === BLOCK USAGE ===

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getMaxBlocks()
{
    return getMaxSizeBytes() / getBlockSize();
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getAllocatedBlocks()
{
    return m_allocatedBlockCount;
}

template <u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getBlockSize()
{
    return sizeof(PoolBlock);
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getRemainingBlocks()
{
    return getMaxBlocks() - getAllocatedBlocks();
}

template <u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::printInfo()
{
    printf("=== POOL ALLOCATOR ===\n");
    printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());
    printf("blocks: [%llu/%llu] %llu free\n", getAllocatedBlocks(), getMaxBlocks(), getRemainingBlocks());
    printf("block size: [%llu bytes] data=%llu nextBlock=%llu\n", getBlockSize(), sizeof(u8[BlockSizeBytes]), sizeof(PoolBlock*));
    printf("======================\n");
}

