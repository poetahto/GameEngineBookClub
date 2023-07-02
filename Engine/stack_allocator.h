#pragma once

#include "types.h"

#define ALLOC_STACK(bytes) g_memoryManager.getStackAllocator()->alloc(bytes)

class StackAllocator
{
public:
    typedef size_t Marker;

    void init(void* baseAddress, size_t maxSizeBytes);
    void* alloc(size_t sizeBytes);
    Marker getMarker();
    void freeToMarker(Marker marker);
    void clear();
    size_t getMaxSize();
    size_t getRemainingBytes();
    size_t getAllocatedBytes();

private:
    U8* m_baseAddress;
    Marker m_topOfStack;
    size_t m_maxSizeBytes;
};

