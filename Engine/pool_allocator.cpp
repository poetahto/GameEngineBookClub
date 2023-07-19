#include <algorithm>
#include <cassert>
#include <new>

#include "pool_allocator.h"

void PoolAllocator::init(void* baseAddress, u64 maxSizeBytes, u64 blockSizeBytes)
{
    m_maxSizeBytes = maxSizeBytes;
    m_blockSizeBytes = std::max(blockSizeBytes, sizeof(PoolBlock)) ;
    m_blocks = new (baseAddress) PoolBlock[getMaxBlocks()];

    // Initializes all blocks to empty states.
    clear();
}

void* PoolAllocator::alloc()
{
    if (m_firstFreeBlock == nullptr)
    {
        // todo: log that we are out of memory
        return nullptr;
    }

    void* result = m_firstFreeBlock;
    m_firstFreeBlock = m_firstFreeBlock->nextFreeBlock;
    m_allocatedBlockCount++;
    return result;
}

void PoolAllocator::free(void* buffer)
{
    assert(buffer != nullptr);
    PoolBlock* prevFreeBlock = m_firstFreeBlock;
    m_firstFreeBlock = static_cast<PoolBlock*>(buffer);
    m_firstFreeBlock->nextFreeBlock = prevFreeBlock;
    m_allocatedBlockCount--;
}

void PoolAllocator::clear()
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

u64 PoolAllocator::getMaxSizeBytes() const
{
    return m_maxSizeBytes;
}

u64 PoolAllocator::getRemainingBytes() const
{
    return getMaxSizeBytes() - getAllocatedBytes();
}

u64 PoolAllocator::getAllocatedBytes() const
{
    return getAllocatedBlocks() * getBlockSizeBytes();
}

// === BLOCK USAGE ===

u64 PoolAllocator::getMaxBlocks() const
{
    return getMaxSizeBytes() / getBlockSizeBytes();
}

u64 PoolAllocator::getAllocatedBlocks() const
{
    return m_allocatedBlockCount;
}

u64 PoolAllocator::getRemainingBlocks() const
{
    return getMaxBlocks() - getAllocatedBlocks();
}

void PoolAllocator::printInfo() const
{
    printf("=== POOL ALLOCATOR ===\n");
    printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());
    printf("blocks: [%llu/%llu] %llu free\n", getAllocatedBlocks(), getMaxBlocks(), getRemainingBlocks());
    printf("block size: [%llu bytes] nextBlock=%llu\n", getBlockSizeBytes(), sizeof(void*));
    printf("======================\n");
}

u64 PoolAllocator::getBlockSizeBytes() const
{
    return m_blockSizeBytes;
}
