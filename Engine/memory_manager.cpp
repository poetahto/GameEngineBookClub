#include "memory_manager.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

void MemoryManager::init()
{
    // todo: should the allocator sizes be hard coded here, or loaded w/ config
    size_t stackSize = 2048;
    size_t oneFrameSize = 1024;
    size_t totalSize = stackSize + oneFrameSize; 

#ifdef _WIN32
    m_baseAddress = VirtualAlloc(
        nullptr, 
        totalSize, 
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
#endif // _WIN32

    // The reason I chose to use an internal stack allocator is so we can 
    // easily align our public-facing allocators, maybe being useful for 
    // ensuring the alignment of pools.
    m_internalAllocator.init(m_baseAddress, totalSize);

    m_stackAllocator.init(m_internalAllocator.alloc(stackSize), stackSize);
    m_oneFrameAllocator.init(m_internalAllocator.alloc(oneFrameSize), oneFrameSize);
}

StackAllocator* MemoryManager::getStackAllocator()
{
    return &m_stackAllocator;
}

StackAllocator* MemoryManager::getOneFrameAllocator()
{
    return &m_oneFrameAllocator;
}

void MemoryManager::shutdown()
{
    m_internalAllocator.clear();

#ifdef _WIN32
    VirtualFree(m_baseAddress, 0, MEM_RELEASE);
#endif // _WIN32
}
