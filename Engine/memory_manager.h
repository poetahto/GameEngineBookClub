#pragma once

#include "stack_allocator.h"

class MemoryManager
{
public:
    void init();
    void shutdown();

    StackAllocator& getStackAllocator();

private:
    StackAllocator m_stackAllocator;
    void* m_baseAddress;
};

