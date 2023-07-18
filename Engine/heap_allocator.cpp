#include <cstdio>
#include <new>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "heap_allocator.h"


void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_baseAddress = static_cast<u8*>(baseAddress);
    m_maxBlockSize = sizeof(MemoryBlock) * 2;
    m_maxAllocations = maxSizeBytes / (m_maxBlockSize + sizeof(HeapPointer));

    // Set aside memory for non-relocatable pointers that can be adjusted when defragmentation.
    u64 pointersSize = m_maxAllocations * sizeof(HeapPointer);
    m_maxSizeBytes = maxSizeBytes - pointersSize;
    m_pointers.init(baseAddress, pointersSize);

    // Start off with a clean heap.
    clear();
}

// todo: alignment
HeapPointer* HeapAllocator::alloc(u64 sizeBytes)
{
    // we only want allocated in chunks the size of our free block - this way we can always have room for free storage
    u64 trueSizeBytes = static_cast<u64>(ceilf(static_cast<f32>(sizeBytes) / sizeof(MemoryBlock))) * sizeof(MemoryBlock);
    m_allocatedBytes += trueSizeBytes;

    // find the block to return for our users
    MemoryBlock* previous = nullptr;
    MemoryBlock* freeBlock = m_freeBlocks;

    while (freeBlock != nullptr)
    {
        if (freeBlock->header.sizeBytes >= trueSizeBytes)
        {
            break;
        }

        previous = freeBlock;
        freeBlock = freeBlock->next;
    }

    if (freeBlock == nullptr)
    {
        // todo: log out-of-memory error (or too fragmented!)
        return nullptr;
    }

    u64 leftoverBytes = freeBlock->header.sizeBytes - trueSizeBytes;
    freeBlock->header.sizeBytes = trueSizeBytes;

    // Check if we need to insert an empty block after the used one.
    if (leftoverBytes > 0)
    {
        void* newFreePointer = reinterpret_cast<u8*>(freeBlock) + trueSizeBytes;
        MemoryBlock* newFreeBlock = new (newFreePointer) MemoryBlock {};
        newFreeBlock->header.sizeBytes = leftoverBytes;
        newFreeBlock->next = freeBlock->next;

        freeBlock->next = newFreeBlock;
    }

    // Make sure the linked list remains valid.
    if (previous != nullptr)
    {
        previous->next = freeBlock->next;
    }
    else // We have no previous element, hence we are the first in the list.
    {
        m_freeBlocks = freeBlock->next;
    }

    // Initialize the used blocks data to default values.
    freeBlock->data = new (&freeBlock->data) u8[sizeBytes] {};

    return new (m_pointers.alloc()) HeapPointer { reinterpret_cast<u8*>(&freeBlock->data) };
}

void HeapAllocator::free(HeapPointer* pointer)
{
    // We get the shifted address back, and must look a bit earlier to find out header information.
    MemoryBlock* newFreeBlock = reinterpret_cast<MemoryBlock*>(pointer->rawPtr - sizeof MemoryBlock::header);

    m_allocatedBytes -= newFreeBlock->header.sizeBytes;
    m_pointers.free(pointer);

    // Check to see if there is a free block following this one.
    u8* nextBlockPointer = reinterpret_cast<u8*>(newFreeBlock) + newFreeBlock->header.sizeBytes;
    MemoryBlock* currentBlock = m_freeBlocks;
    MemoryBlock* nextBlock = nullptr;
    MemoryBlock* prevBlock = nullptr;

    while (currentBlock != nullptr)
    {
        nextBlock = currentBlock->next;

        if (reinterpret_cast<u8*>(nextBlock) == nextBlockPointer)
        {
            // We have a neighbor, which is "next" block.
            prevBlock = currentBlock;
            break;
        }

        if (reinterpret_cast<u8*>(currentBlock) == nextBlockPointer)
        {
            // We are the "next" block.
            nextBlock = currentBlock;
            break;
        }

        currentBlock = nextBlock;
    }

    // Merge surrounding free blocks.
    if (nextBlock != nullptr)
    {
        merge(newFreeBlock, nextBlock);
    }

    if (prevBlock != nullptr)
    {
        merge(prevBlock, newFreeBlock);
    }
}

void HeapAllocator::clear()
{
    m_allocatedBytes = 0;
    m_pointers.clear();

    // Create a new, empty free store right after the pointers.
    m_freeBlocks = new (m_baseAddress + m_pointers.getMaxSizeBytes()) MemoryBlock {};
    m_freeBlocks->header.sizeBytes = m_maxSizeBytes;
    m_freeBlocks->next = nullptr;
}

void HeapAllocator::defragment()
{
    MemoryBlock* free = m_freeBlocks;
    MemoryBlock* next = free->next;

    if (free->next != nullptr)
    {
        // Save state before memory shuffles around.
        u8* usedPointer = reinterpret_cast<u8*>(free) + free->header.sizeBytes;
        MemoryBlock* used = reinterpret_cast<MemoryBlock*>(usedPointer);
        u64 freeSize = free->header.sizeBytes;

        // Copy over the data from the used block into the free block.
        memmove(free, used, used->header.sizeBytes);

        MemoryBlock* newFree = new (reinterpret_cast<u8*>(free) + free->header.sizeBytes) MemoryBlock {};
        newFree->header.sizeBytes = freeSize;
        newFree->next = next;

        // Now check if the new free space needs to be merged.
        if (next != nullptr)
        {
            newFree->next = next->next;
            merge(newFree, next);
        }
        else
        {
            newFree->next = next;
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

    MemoryBlock* currentBlock = m_freeBlocks;

    while (currentBlock != nullptr)
    {
        u64 offsetFromBase = reinterpret_cast<uintptr_t>(currentBlock) - reinterpret_cast<uintptr_t>(m_freeBlocks);
        printf("\t%p | offset +%-5llu Free Block [%llu bytes]\n", static_cast<void*>(currentBlock), offsetFromBase, currentBlock->header.sizeBytes);
        currentBlock = currentBlock->next;
    }

    m_pointers.printInfo();
    printf("^^^ heap pointers ^^^\n\n");

    printf("======================\n");
}

// === MEMBER FUNCTIONS ===

void HeapAllocator::merge(MemoryBlock* first, MemoryBlock* second) // static
{
    // We should only ever merge consecutive blocks together.
    assert(reinterpret_cast<u8*>(first) + first->header.sizeBytes == reinterpret_cast<u8*>(second));

    first->header.sizeBytes = first->header.sizeBytes + second->header.sizeBytes;
    first->next = second->next;
}
