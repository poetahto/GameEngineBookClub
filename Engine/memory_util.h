#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include <cassert>
#include "types.h"

// We probably never need to align greater than 65,536 bytes.
typedef u16 AlignmentAmount;

// The reason for wrapping the size inside a struct is readability:
// it makes it easier to distinguish between the two sizes in the overloaded
// alloc function.
struct Align
{
    explicit Align(AlignmentAmount amount) : amount{amount}
    {
    }

    const AlignmentAmount amount;
};

// Adapted from https://stackoverflow.com/questions/42093360/how-to-check-if-a-pointer-points-to-a-properly-aligned-memory-location
inline bool isAligned(const void* pointer, AlignmentAmount alignment)
{
    uintptr_t pointerAddr = reinterpret_cast<uintptr_t>(pointer);
    return !(pointerAddr % alignment);
}

// The number of bytes needed to store some data aligned to a boundary.
inline u64 getAlignedSize(u64 sizeBytes, Align align = Align(8))
{
    return sizeBytes + align.amount - 1;
}

struct AlignmentResult
{
    void* alignedPointer;
    AlignmentAmount bias;
    u8 pad[1];
};

// This alignment strategy is adapted from "Game Engine Architecture" pg. 431
// With some additional compiler-friendly edits thanks to clang
// https://clang.llvm.org/extra/clang-tidy/checks/performance/no-int-to-ptr.html
inline AlignmentResult align(u8* basePointer, Align align = Align(8))
{
    const AlignmentAmount mask = align.amount - 1;
    assert((align.amount & mask) == 0); // make sure alignment is a power of 2

    const uintptr_t basePointerAddr = reinterpret_cast<uintptr_t>(basePointer);
    const uintptr_t alignedPointerAddr = basePointerAddr + mask & ~mask; // bitwise magic
    const AlignmentAmount bias = static_cast<AlignmentAmount>(alignedPointerAddr - basePointerAddr);

    return AlignmentResult
    {
        basePointer + bias,
        bias,
    };
}

#endif // MEMORY_UTIL_H
