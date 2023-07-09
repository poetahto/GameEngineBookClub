#include <cassert>
#include <new>
#include "pool_allocator.h"

template<size_t blockSizeBytes>
void PoolAllocator<blockSizeBytes>::init(void* baseAddress, size_t maxSizeBytes)
{
    size_t numBlocks = maxSizeBytes / blockSizeBytes;
    m_blocks = new (baseAddress) PoolBlock[numBlocks];

    // Initialize our free list, so every block points to a neighbor.
    for (int i = 0; i < numBlocks - 1; i++)
    {
        m_blocks[i].nextFreeBlock = &m_blocks[i + 1];
    }

    // The final block has no neighbor.
    m_blocks[numBlocks - 1].nextFreeBlock = nullptr;

    m_firstFreeBlock = &m_blocks[0];
};

template<size_t blockSizeBytes>
void* PoolAllocator<blockSizeBytes>::alloc()
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

template<size_t blockSizeBytes>
void PoolAllocator<blockSizeBytes>::free(void* buffer)
{
    assert(buffer != nullptr);
    PoolBlock* prevfreeBlock = m_firstFreeBlock;
    m_firstFreeBlock = static_cast<PoolBlock*>(buffer);
    m_firstFreeBlock->nextFreeBlock = prevfreeBlock;
};

