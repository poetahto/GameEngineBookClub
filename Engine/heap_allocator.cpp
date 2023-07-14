#include <cstdio>
#include <new>
#include <cassert>
#include <cstring>
#include "heap_allocator.h"

void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_maxAllocations = maxSizeBytes / (sizeof(MemoryBlock) + sizeof(MemoryBlock));
    m_firstMemoryBlock = static_cast<MemoryBlock*>(baseAddress);
    u64 memoryBlocksSize = m_maxAllocations * sizeof(MemoryBlock);
    m_maxSizeBytes = maxSizeBytes - memoryBlocksSize;
    m_memoryBlocks.init(m_firstMemoryBlock, memoryBlocksSize);

    // Start off with a clean heap.
    clear();
}

MemoryBlock* HeapAllocator::alloc(u64 sizeBytes)
{
    m_allocatedBytes += sizeBytes + sizeof(MemoryBlock);

    // Saving old state before we shuffle memory around.
    MemoryBlock* oldFreeBlock = getFreeBlock(sizeBytes);

    if (oldFreeBlock == nullptr)
    {
        // todo: log out-of-memory error (or too fragmented!)
        return nullptr;
    }

    u64 leftoverBytes = oldFreeBlock->sizeBytes - sizeBytes;

    // Update block to reflect used state.
    oldFreeBlock->isFree = false;
    oldFreeBlock->sizeBytes = sizeBytes;

    // Check if we need to insert an empty block after the used one.
    if (leftoverBytes > 0)
    {
        MemoryBlock* newFreeBlock = new(m_memoryBlocks.alloc()) MemoryBlock
        {
            true, leftoverBytes, oldFreeBlock->next, oldFreeBlock, oldFreeBlock->address + sizeBytes
        };

        if (oldFreeBlock->next != nullptr)
        {
            oldFreeBlock->next->previous = newFreeBlock;
        }

        oldFreeBlock->next = newFreeBlock;
    }

    return oldFreeBlock;
}

void HeapAllocator::free(MemoryBlock* block)
{
    MemoryBlock* newFreeBlock = block;
    newFreeBlock->isFree = true;

    m_allocatedBytes -= newFreeBlock->sizeBytes + sizeof(MemoryBlock);

    MemoryBlock* nextBlock = newFreeBlock->next;
    MemoryBlock* previousBlock = newFreeBlock->previous;

    if (nextBlock != nullptr && nextBlock->isFree)
    {
        merge(newFreeBlock, nextBlock);
    }

    if (previousBlock != nullptr && previousBlock->isFree)
    {
        merge(previousBlock, newFreeBlock);
    }
}

void HeapAllocator::clear()
{
    m_allocatedBytes = 0;
    m_memoryBlocks.clear();

    // The free store begins right after the memory blocks.
    u8* address = reinterpret_cast<u8*>(m_firstMemoryBlock + m_maxAllocations);

    m_firstMemoryBlock = new (m_memoryBlocks.alloc()) MemoryBlock
    {
        true, m_maxSizeBytes, nullptr, nullptr, address
    };
}

void HeapAllocator::defragment()
{
    MemoryBlock* free = getFreeBlock();
    MemoryBlock* used = free->next;

    if (used != nullptr)
    {
        // Copy over the data from the used block into the free block.
        memmove(free->address, used->address, used->sizeBytes);

        // Swap the addresses
        used->address = free->address;
        free->address = used->address + used->sizeBytes;

        // BEFORE: previous <-> free <-> used <-> next
        // AFTER:  previous <-> used <-> free <-> next
        MemoryBlock* previous = free->previous;
        MemoryBlock* next = used->next;

        previous->next = used;
        used->previous = previous;
        used->next = free;
        free->previous = used;
        free->next = next;
        next->previous = free;

        // Now check if the new free space needs to be merged.
        if (next != nullptr && next->isFree)
        {
            merge(free, next);
        }
    }
}

// === BYTE USAGE ===

u64 HeapAllocator::getMaxSizeBytes() const
{
    return m_maxSizeBytes;
}

u64 HeapAllocator::getAllocatedBytes() const
{
    return m_allocatedBytes;
}

u64 HeapAllocator::getRemainingBytes() const
{
    return getMaxSizeBytes() - getAllocatedBytes();
}

// === DEBUGGING ===

void HeapAllocator::printInfo() const
{
    printf("=== HEAP ALLOCATOR ===\n");
    printf("bytes: [%llu/%llu] %llu free\n", getAllocatedBytes(), getMaxSizeBytes(), getRemainingBytes());

    MemoryBlock* currentBlock = m_firstMemoryBlock;

    while (currentBlock != nullptr)
    {
        u64 offsetFromBase = reinterpret_cast<uintptr_t>(currentBlock) - reinterpret_cast<uintptr_t>(
            m_firstMemoryBlock);
        printf("\t%p | offset +%-5llu %5s Block [%llu bytes]\n", static_cast<void*>(currentBlock), offsetFromBase,
               currentBlock->isFree ? "Free" : "Used", currentBlock->sizeBytes);
        currentBlock = currentBlock->next;
    }

    printf("^^^ heap pointers ^^^\n");
    m_memoryBlocks.printInfo();
    printf("^^^ memory blocks ^^^\n\n");

    printf("======================\n");
}

// === MEMBER FUNCTIONS ===

// todo: pretty slow, make faster w/ separate free list? multiple free lists?
MemoryBlock* HeapAllocator::getFreeBlock(u64 sizeBytes) const
{
    MemoryBlock* current = m_firstMemoryBlock;

    while (current != nullptr)
    {
        if (current->isFree && current->sizeBytes >= sizeBytes)
        {
            return current;
        }

        current = current->next;
    }

    return nullptr;
}

void HeapAllocator::merge(MemoryBlock* first, MemoryBlock* second)
{
    // We should only ever merge two free blocks together.
    assert(first->isFree);
    assert(second->isFree);

    // We should only ever merge consecutive blocks together.
    assert(first->next == second);
    assert(second->previous == first);

    first->sizeBytes = first->sizeBytes + second->sizeBytes;

    // BEFORE: first <-> second <-> next
    // AFTER:  first <-> next
    MemoryBlock* next = second->next;
    first->next = next;

    if (next != nullptr)
    {
        next->previous = first;
    }

    m_memoryBlocks.free(second);
}
