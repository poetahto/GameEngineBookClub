#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "stack_allocator.h"

// todo: plan a way to sneak profiling tools into the allocation calls?
// need some way to grab __file__ ect. info without cluttering, probably
// a macro at some point for allocations

/// <summary>
/// Distributes total memory resources into different allocators,
/// and provides convenient access to them.
/// </summary>
class MemoryManager
{
public:
    void init();
    void shutdown();

    StackAllocator* getStackAllocator();
    StackAllocator* getOneFrameAllocator();

private:
    void* m_baseAddress {};
    StackAllocator m_internalAllocator {};

    StackAllocator m_stackAllocator {};
    StackAllocator m_oneFrameAllocator {};
};

#endif // MEMORY_MANAGER_H
