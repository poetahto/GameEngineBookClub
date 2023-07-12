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
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // Block usage
    u64 getMaxBlocks() const;
    u64 getRemainingBlocks() const;
    u64 getAllocatedBlocks() const;

    // Debugging
    static u64 getBlockSize();
    void printInfo() const;

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
