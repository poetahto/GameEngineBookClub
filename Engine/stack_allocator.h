#pragma once

#include "types.h"

// todo: move this somewhere else
#define ALLOC_STACK(bytes) g_memoryManager.getStackAllocator()->alloc(bytes)

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
    size_t getMaxSize();
    size_t getRemainingBytes();
    size_t getAllocatedBytes();

private:
    U8* m_baseAddress;
    Marker m_topOfStack;
    size_t m_maxSizeBytes;
};

struct Align
{
    Align(size_t amount) : amount { amount } 
    {
    }

    size_t amount;
};

