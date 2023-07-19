#include <cassert>
#include <cstdio>

#include "stack_allocator.h"
#include "memory_util.h"

// todo: create double-ended stack allocator (kinda low priority, looks pretty simple and similar to this stack allocator

void StackAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_baseAddress = static_cast<u8*>(baseAddress);
    m_maxSizeBytes = maxSizeBytes;
    m_topOfStack = 0;
}

void* StackAllocator::alloc(u64 sizeBytes, Align alignment)
{
    u64 alignedSize = getAlignedSize(sizeBytes, alignment);

    if (m_topOfStack + alignedSize > m_maxSizeBytes)
    {
        // todo: logging warnings for this case, once we have logging, also conditionally compile out?
        return nullptr;
    }

    m_topOfStack += alignedSize;
    return align(m_baseAddress + m_topOfStack, alignment);
}

StackAllocator::Marker StackAllocator::getMarker() const
{
    return m_topOfStack;
}

void StackAllocator::freeToMarker(Marker marker)
{
    // We know that we give out valid markers, just asserting that our
    // users give us the same treatment...
    assert(marker <= m_maxSizeBytes);

    m_topOfStack = marker;
}

void StackAllocator::clear()
{
    m_topOfStack = 0;
}

u64 StackAllocator::getMaxSizeBytes() const
{
    return m_maxSizeBytes;
}

u64 StackAllocator::getRemainingBytes() const
{
    return m_maxSizeBytes - m_topOfStack;
}

// note: this is the exact same as "getMarker()", but the result can be
// interpreted as more useful data.
u64 StackAllocator::getAllocatedBytes() const
{
    return m_topOfStack;
}

void StackAllocator::printInfo() const
{
    printf("=== STACK ALLOCATOR ===\n");
    printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());
    printf("=======================\n");
}
