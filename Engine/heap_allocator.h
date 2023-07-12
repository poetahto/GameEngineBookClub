#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "types.h"

// todo: implement functions
// todo: defragmentation? probably comes after basic implementation (should be able to spread over several frames, ref. book)

class HeapAllocator
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

    // Debugging
    void printInfo() const;
};

#endif // HEAP_ALLOCATOR_H
