#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include <cstdint>

// Adapted from https://stackoverflow.com/questions/42093360/how-to-check-if-a-pointer-points-to-a-properly-aligned-memory-location
inline bool isAligned(const void* pointer, size_t align)
{
    uintptr_t pointerAddr = reinterpret_cast<uintptr_t>(pointer);
    return !(pointerAddr % align);
}

#endif // MEMORY_UTIL_H
