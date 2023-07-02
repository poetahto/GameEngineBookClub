#include "memory_manager.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

MemoryManager g_memoryManager;

void MemoryManager::init()
{
    const size_t stackAllocatorSize = 2048;
    size_t totalSize = stackAllocatorSize; // for in the future, when we implement more allocators.

#ifdef _WIN32
    m_baseAddress = VirtualAlloc(nullptr, totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif // _WIN32

    U8* address = static_cast<U8*>(m_baseAddress);

    m_stackAllocator.init(address, stackAllocatorSize);
    address += stackAllocatorSize;

    // ... and so on, when we have more allocators to implement.
}

void MemoryManager::shutdown()
{
#ifdef _WIN32
    VirtualFree(m_baseAddress, 0, MEM_RELEASE);
#endif // _WIN32
}

StackAllocator& MemoryManager::getStackAllocator()
{
    return m_stackAllocator;
}
