#include <stdio.h>
#include <new>
#include "../Engine/memory_manager.h"

class Foo
{
    int bar = 0xdeadbeef;
};

extern MemoryManager g_memoryManager;

int main()
{
    g_memoryManager.init();

    StackAllocator stackAllocator = g_memoryManager.getStackAllocator();
    void* buffer = stackAllocator.alloc(sizeof(Foo));
    Foo* foo = new (buffer) Foo();

    g_memoryManager.shutdown();
    return 0;
}