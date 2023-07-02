#include "memory_manager.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// Helper methods just to make the allocator initialization code more concise. 
// todo: Should these be macros?
inline void* MemoryManager::getBuffer(size_t size)
{
    return m_internalAllocator.alloc(size);
}

inline void* MemoryManager::getBuffer(size_t size, Align align)
{
    return m_internalAllocator.alloc(size, align);
}

void MemoryManager::init()
{
    // todo: should the allocator sizes be hard coded here, or loaded w/ config
    const size_t stackSize = 2048;
    m_totalSize = stackSize; 

#ifdef _WIN32
    m_baseAddress = VirtualAlloc(
        nullptr, 
        m_totalSize, 
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
#endif // _WIN32

    // The reason I chose to use an internal stack allocator is so we can 
    // easily align our public-facing allocators, maybe being useful for 
    // ensuring the alignment of pools.
    m_internalAllocator.init(m_baseAddress, m_totalSize);

    m_stackAllocator.init(getBuffer(stackSize), stackSize);
    // ... and so on, when we have more allocators to implement.
}

void MemoryManager::shutdown()
{
    m_internalAllocator.clear();

#ifdef _WIN32
    VirtualFree(m_baseAddress, 0, MEM_RELEASE);
#endif // _WIN32
}

StackAllocator* MemoryManager::getStackAllocator()
{
    return &m_stackAllocator;
}

size_t MemoryManager::getTotalSize()
{
    return m_totalSize;
}
