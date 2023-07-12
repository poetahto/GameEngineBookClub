#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "types.h"

template<u64 BlockSizeBytes>
class PoolAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes);
    void* alloc();
    void free(void* buffer);
    void clear();

    // Byte usage
    u64 getMaxSizeBytes();
    u64 getRemainingBytes();
    u64 getAllocatedBytes();

    // Block usage
    u64 getMaxBlocks();
    u64 getRemainingBlocks();
    u64 getAllocatedBlocks();
    u64 getBlockSize();

    // Debugging
    void printInfo();

private:
    struct PoolBlock;

    PoolBlock* m_blocks {};
    PoolBlock* m_firstFreeBlock {};
    u64 m_maxSizeBytes {};
    u64 m_allocatedBlockCount {};

    struct PoolBlock
    {
        union
        {
            u8 data[BlockSizeBytes];
            PoolBlock* nextFreeBlock;
        };
    };
};

#include "pool_allocator.tpp"

#endif // POOL_ALLOCATOR_H
