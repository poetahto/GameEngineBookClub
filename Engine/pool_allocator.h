#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "types.h"

class PoolAllocator
{
public:
    void init(void* baseAddress, u64 maxSizeBytes, u64 blockSizeBytes);
    void* alloc(); // todo: alloc based alignment here, like in the book?
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
    void printInfo() const;
    u64 getBlockSizeBytes() const;

private:
    // todo: look at struct memory layout, explicitly show padding (from book)
    struct PoolBlock
    {
        // todo: look into index-based free list tracking? so no need for 8 byte pointers? MORE templating?
        PoolBlock* nextFreeBlock;
    };

    PoolBlock* m_blocks {};
    PoolBlock* m_firstFreeBlock {};
    u64 m_blockSizeBytes {};
    u64 m_maxSizeBytes {};
    u64 m_allocatedBlockCount {};
};

#endif // POOL_ALLOCATOR_H
