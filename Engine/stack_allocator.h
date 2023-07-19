#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "types.h"
#include "memory_util.h"

// todo: document public api: i think its stable enough to start locking in

class StackAllocator
{
public:
    typedef u64 Marker;

    void init(void* baseAddress, u64 maxSizeBytes);
    void* alloc(u64 sizeBytes, Align align = Align(8));
    Marker getMarker() const;
    void freeToMarker(Marker marker);
    void clear();

    // Byte usage
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // Debugging
    void printInfo() const;

private:
    u8* m_baseAddress{};
    u64 m_maxSizeBytes{};
    Marker m_topOfStack{};
};

#endif // STACK_ALLOCATOR_H
