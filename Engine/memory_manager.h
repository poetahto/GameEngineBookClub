#pragma once

#include "stack_allocator.h"

class MemoryManager
{
public:
    void init();
    void shutdown();

    StackAllocator* getStackAllocator();
    size_t getTotalSize();

private:
    StackAllocator m_stackAllocator;
    void* m_baseAddress;
    size_t m_totalSize;
};

