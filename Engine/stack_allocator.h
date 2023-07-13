#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "types.h"

// todo: move this somewhere else? or is it even necessary? (also, didn't make for any other allocator)
#define ALLOC_STACK(bytes) g_memoryManager.getStackAllocator()->alloc(bytes)

// todo: document public api: i think its stable enough to start locking in

struct Align;

class StackAllocator
{
public:
    typedef u64 Marker;

    void init(void* baseAddress, u64 maxSizeBytes);
    void* alloc(u64 sizeBytes);
    void* alloc(u64 sizeBytes, Align align);
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

// The reason for wrapping the size inside a struct is readability:
// it makes it easier to distinguish between the two sizes in the overloaded
// alloc function.
struct Align
{
    explicit Align(u64 amount) : amount{amount}
    {
    }

    const u64 amount;
};

#endif // STACK_ALLOCATOR_H
