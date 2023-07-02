#include <cassert>
#include <stdlib.h>
#include "stack_allocator.h"

void StackAllocator::init(void* baseAddress, size_t sizeBytes)
{
    m_baseAddress = static_cast<U8*>(baseAddress);
    m_topOfStack = 0;
    m_maxSizeBytes = sizeBytes;
}

void* StackAllocator::alloc(size_t bytes)
{
    if (m_topOfStack + bytes > m_maxSizeBytes)
    {
        // todo: logging warnings for this case, once we have logging
        return nullptr;
    }

    void* result = m_baseAddress + m_topOfStack;
    m_topOfStack += bytes; 
    return result;
}

void* StackAllocator::alloc(size_t sizeBytes, Align align)
{
    void* pointer = alloc(sizeBytes + align.amount - 1);

    // we have to reinterept our pointer into an integral type that we can perform bitwise operations on.
    uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
    size_t mask = align.amount - 1;
    assert((align.amount & mask) == 0); // make sure alignment is a power of 2
    addr = (addr + mask) & ~mask;
    return reinterpret_cast<void*>(addr);
}

StackAllocator::Marker StackAllocator::getMarker()
{
    return m_topOfStack;
}

void StackAllocator::freeToMarker(Marker marker)
{
    assert(marker >= 0);
    assert(marker <= m_maxSizeBytes);
    m_topOfStack = marker;
}

void StackAllocator::clear()
{
    m_topOfStack = 0;
}

size_t StackAllocator::getMaxSize()
{
    return m_maxSizeBytes;
}

size_t StackAllocator::getRemainingBytes()
{
    return m_maxSizeBytes - m_topOfStack;
}

size_t StackAllocator::getAllocatedBytes()
{
    return m_topOfStack;
}
