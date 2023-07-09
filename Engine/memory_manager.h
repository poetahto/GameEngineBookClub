#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <new> // so users of this class can get placement new by default.
#include "stack_allocator.h"

// todo: plan a way to sneak profiling tools into the allocation calls? 
// need some way to grab __file__ ect. info without cluttering, probably 
// a macro at some point for allocations

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
    StackAllocator* getOneFrameAllocator();

private:
    StackAllocator m_internalAllocator;
    void* m_baseAddress;

    StackAllocator m_stackAllocator;
    StackAllocator m_oneFrameAllocator;
};

#endif // MEMORY_MANAGER_H
