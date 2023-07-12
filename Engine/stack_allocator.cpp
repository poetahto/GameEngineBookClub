#include <cassert>
#include <cstdio>
#include "stack_allocator.h"

// todo: create double-ended stack allocator (kinda low priority, looks pretty simple and similar to this stack allocator

void StackAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_baseAddress = static_cast<u8*>(baseAddress);
    m_maxSizeBytes = maxSizeBytes;
    m_topOfStack = 0;
}

void* StackAllocator::alloc(u64 sizeBytes)
{
    if (m_topOfStack + sizeBytes > m_maxSizeBytes)
    {
        // todo: logging warnings for this case, once we have logging
        return nullptr;
    }

    void* result = m_baseAddress + m_topOfStack;
    m_topOfStack += sizeBytes;
    return result;
}

// This alignment strategy is adapted from "Game Engine Architecture" pg. 431
// With some additional compiler-friendly edits thanks to clang
// https://clang.llvm.org/extra/clang-tidy/checks/performance/no-int-to-ptr.html
void* StackAllocator::alloc(u64 sizeBytes, Align align)
{
    const u64 mask = align.amount - 1;
    assert((align.amount & mask) == 0); // make sure alignment is a power of 2

    u8* buffer = static_cast<u8*>(alloc(sizeBytes + align.amount - 1));
    const uintptr_t bufferAddress = reinterpret_cast<uintptr_t>(buffer);
    const uintptr_t alignedBufferAddress = bufferAddress + mask & ~mask; // bitwise magic
    const uintptr_t bias = alignedBufferAddress - bufferAddress;
    return buffer + bias;
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
