#include "memory_util.h"

using namespace memory;

// Adapted from https://stackoverflow.com/questions/42093360/how-to-check-if-a-pointer-points-to-a-properly-aligned-memory-location
bool memory::isAligned(const void* pointer, Alignment align)
{
    uintptr_t pointerAddr = reinterpret_cast<uintptr_t>(pointer);
    return !(pointerAddr % align);
}

u64 memory::getAlignedSize(u64 sizeBytes, Alignment align)
{
    return sizeBytes + align - 1;
}

// This alignment strategy is adapted from "Game Engine Architecture" pg. 431
// With some additional compiler-friendly edits thanks to clang
// https://clang.llvm.org/extra/clang-tidy/checks/performance/no-int-to-ptr.html
u8* memory::alignPointer(u8* basePointer, Alignment align)
{
    const Alignment mask = align - 1;
    assert((align & mask) == 0); // make sure alignment is a power of 2

    const uintptr_t basePointerAddr = reinterpret_cast<uintptr_t>(basePointer);
    const uintptr_t alignedPointerAddr = basePointerAddr + mask & ~mask; // bitwise magic
    const Alignment bias = static_cast<Alignment>(alignedPointerAddr - basePointerAddr);
    return basePointer + bias;
}
