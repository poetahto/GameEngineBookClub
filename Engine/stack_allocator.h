#pragma once

#include "types.h"

class StackAllocator
{
public:
    typedef size_t Marker;

    void init(void* baseAddress, size_t maxSizeBytes);
    void* alloc(size_t sizeBytes);
    Marker getMarker();
    void freeToMarker(Marker marker);
    void clear();

private:
    U8* m_baseAddress;
    Marker m_topOfStack;
    size_t m_maxSizeBytes;
};

