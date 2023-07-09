#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "types.h"

template<size_t blockSizeBytes>
class PoolAllocator
{
public:
    void init(void* baseAddress, size_t maxSizeBytes);
    void* alloc();
    void free(void* buffer);

private:
    struct PoolBlock;

    PoolBlock* m_blocks;
    PoolBlock* m_firstFreeBlock;

    struct PoolBlock
    {
        union
        {
            u8 data[blockSizeBytes];
            PoolBlock* nextFreeBlock;
        };
    };
};

#endif // POOL_ALLOCATOR_H
