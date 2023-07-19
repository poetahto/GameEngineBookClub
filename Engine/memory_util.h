#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include <cassert>
#include "types.h"

namespace memory
{
    // We probably never need to align greater than 65,536 bytes.
    typedef u16 Alignment;

    constexpr Alignment DEFAULT_ALIGNMENT = 8;

    // Adapted from https://stackoverflow.com/questions/42093360/how-to-check-if-a-pointer-points-to-a-properly-aligned-memory-location
    inline bool isAligned(const void* pointer, Alignment alignment)
    {
        uintptr_t pointerAddr = reinterpret_cast<uintptr_t>(pointer);
        return !(pointerAddr % alignment);
    }

    // The number of bytes needed to store some data aligned to a boundary.
    inline u64 getAlignedSize(u64 sizeBytes, Alignment alignment = DEFAULT_ALIGNMENT)
    {
        return sizeBytes + alignment - 1;
    }

    // This alignment strategy is adapted from "Game Engine Architecture" pg. 431
    // With some additional compiler-friendly edits thanks to clang
    // https://clang.llvm.org/extra/clang-tidy/checks/performance/no-int-to-ptr.html
    inline u8* alignPointer(u8* basePointer, Alignment alignment = DEFAULT_ALIGNMENT)
    {
        const Alignment mask = alignment - 1;
        assert((alignment & mask) == 0); // make sure alignment is a power of 2

        const uintptr_t basePointerAddr = reinterpret_cast<uintptr_t>(basePointer);
        const uintptr_t alignedPointerAddr = basePointerAddr + mask & ~mask; // bitwise magic
        const Alignment bias = static_cast<Alignment>(alignedPointerAddr - basePointerAddr);
        return basePointer + bias;
    }
} // namespace memory

// This is templated in-case we need to change the strategy for getting type alignment.
template <class T>
memory::Alignment align()
{
    return alignof(T);
}

#endif // MEMORY_UTIL_H
