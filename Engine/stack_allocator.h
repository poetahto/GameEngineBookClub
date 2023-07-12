#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "types.h"

// todo: move this somewhere else? or is it even necesarry?
#define ALLOC_STACK(bytes) g_memoryManager.getStackAllocator()->alloc(bytes)

// todo: document public api: i think its stable enough to start locking in

struct Align;

class StackAllocator
{
public:
    typedef size_t Marker;

    void init(void* baseAddress, size_t maxSizeBytes);
    void* alloc(size_t sizeBytes);
    void* alloc(size_t sizeBytes, Align align);

    Marker getMarker();
    void freeToMarker(Marker marker);
    void clear();
    size_t getMaxSizeBytes();
    size_t getRemainingBytes();
    size_t getAllocatedBytes();

private:
    u8* m_baseAddress;
    Marker m_topOfStack;
    size_t m_maxSizeBytes;
};

// The reason for wrapping the size inside a struct is readability:
// it makes it easier to distinguish between the two sizes in the overloaded 
// alloc function.
struct Align
{
    Align(size_t amount) : amount { amount } 
    {
    }

    size_t amount;
};

#endif // STACK_ALLOCATOR_H 
