#include <cstdio>
#include <cassert>
#include "pool_allocator.h"

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::init(void* baseAddress, u64 maxSizeBytes)
{
    m_maxSizeBytes = maxSizeBytes;
    m_blocks = new (baseAddress) PoolBlock[getMaxBlocks()];

    // Initializes all blocks to empty states.
    clear();
}

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
    m_allocatedBlockCount++;
    return result;
}

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::free(void* buffer)
{
    assert(buffer != nullptr);
    PoolBlock* prevFreeBlock = m_firstFreeBlock;
    m_firstFreeBlock = static_cast<PoolBlock*>(buffer);
    m_firstFreeBlock->nextFreeBlock = prevFreeBlock;
    m_allocatedBlockCount--;
}

template<u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::clear()
{
    const u64 numBlocks = getMaxBlocks();

    // Initialize our free list, so every block points to a neighbor.
    for (u64 i = 0; i < numBlocks - 1; i++)
    {
        m_blocks[i].nextFreeBlock = &m_blocks[i + 1];
    }

    m_blocks[numBlocks - 1].nextFreeBlock = nullptr; // The final block has no neighbor.
    m_firstFreeBlock = &m_blocks[0];
    m_allocatedBlockCount = 0;
}

// === BYTE USAGE ===

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getMaxSizeBytes() const
{
    return m_maxSizeBytes;
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getRemainingBytes() const
{
    return getMaxSizeBytes() - getAllocatedBytes();
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getAllocatedBytes() const
{
    return getAllocatedBlocks() * getBlockSizeBytes();
}

// === BLOCK USAGE ===

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getMaxBlocks() const
{
    return getMaxSizeBytes() / getBlockSizeBytes();
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getAllocatedBlocks() const
{
    return m_allocatedBlockCount;
}

template<u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getRemainingBlocks() const
{
    return getMaxBlocks() - getAllocatedBlocks();
}

template <u64 BlockSizeBytes>
u64 PoolAllocator<BlockSizeBytes>::getBlockSizeBytes() // static
{
    return sizeof(PoolBlock);
}

template <u64 BlockSizeBytes>
void PoolAllocator<BlockSizeBytes>::printInfo() const
{
    printf("=== POOL ALLOCATOR ===\n");
    printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());
    printf("blocks: [%llu/%llu] %llu free\n", getAllocatedBlocks(), getMaxBlocks(), getRemainingBlocks());
    printf("block size: [%llu bytes] data=%llu nextBlock=%llu\n", getBlockSizeBytes(), sizeof(u8[BlockSizeBytes]), sizeof(void*));
    printf("======================\n");
}

