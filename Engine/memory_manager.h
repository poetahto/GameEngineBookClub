#pragma once

#include <new> // so users of this class can get placement new by default.
#include "stack_allocator.h"

/// <summary>
/// Distributes total memory resources into different allocators,
/// and provides convienient access to them.
/// </summary>
class MemoryManager
{
public:
    void init();
    void shutdown();

    StackAllocator* getStackAllocator();
    size_t getTotalSize();

private:
    inline void* getBuffer(size_t size);
    inline void* getBuffer(size_t size, Align align);
    StackAllocator m_stackAllocator;
    StackAllocator m_internalAllocator;
    void* m_baseAddress;
    size_t m_totalSize;
};

