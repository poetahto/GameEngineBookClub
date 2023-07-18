#include <cstdio>
#include <new>
#include <cassert>
#include <cstring>
#include <algorithm>

#include "heap_allocator.h"

void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_baseAddress = static_cast<u8*>(baseAddress);
    m_maxAllocations = maxSizeBytes / (sizeof(MemoryBlock) + sizeof(HeapPointer) + 1);

    // Set aside memory for non-relocatable pointers that can be adjusted when defragmentation.
    u64 pointersSize = m_maxAllocations * sizeof(HeapPointer);
    m_maxSizeBytes = maxSizeBytes - pointersSize;
    m_pointers.init(baseAddress, pointersSize);

    // Start off with a clean heap.
    clear();
}

// todo: alignment
// worst-case O(n) time operation, best-case constant-time operation, n = # of free blocks
// becomes faster the less fragmented the heap is.
HeapPointer* HeapAllocator::alloc(u64 sizeBytes)
{
    // we only want allocated in chunks the size of our free block - this way we can always have room for free storage
    u64 trueSizeBytes = static_cast<u64>(ceilf(static_cast<f32>(sizeBytes) / sizeof(MemoryBlock))) * sizeof(MemoryBlock);
    m_allocatedBytes += trueSizeBytes + sizeof(HeapPointer);

    // find the block to return for our users
    MemoryBlock* freeBlock = m_freeBlocks;
    MemoryBlock* prevBlock = nullptr;

    while (freeBlock != nullptr)
    {
        if (freeBlock->header.sizeBytes >= trueSizeBytes)
        {
            break;
        }

        prevBlock = freeBlock;
        freeBlock = freeBlock->freeData.next;
    }

    // We failed to find a free block that's big enough.
    if (freeBlock == nullptr)
    {
        // todo: log out-of-memory error (or too fragmented!)
        return nullptr;
    }

    u64 leftoverBytes = freeBlock->header.sizeBytes - trueSizeBytes;
    freeBlock->header.sizeBytes = trueSizeBytes;
    m_freeBlockCount--;
    u8* freeBlockDataPointer = reinterpret_cast<u8*>(&freeBlock->freeData);

    // Check if we need to insert an empty block after the used one.
    if (leftoverBytes > 0)
    {
        void* newFreePointer = reinterpret_cast<u8*>(freeBlock) + trueSizeBytes;
        MemoryBlock* newFreeBlock = new (newFreePointer) MemoryBlock {};
        newFreeBlock->header.sizeBytes = leftoverBytes;
        newFreeBlock->freeData.next = freeBlock->freeData.next;
        m_freeBlockCount++;

        freeBlock->freeData.next = newFreeBlock;
    }

    // Make sure the linked list remains valid.
    if (prevBlock != nullptr)
    {
        prevBlock->freeData.next = freeBlock->freeData.next;
    }
    else // We have no previous element, hence we are the first in the list.
    {
        m_freeBlocks = freeBlock->freeData.next;
    }

    return new (m_pointers.alloc()) HeapPointer { freeBlockDataPointer };
}

// worst-case O(n) time operation, best-case constant-time operation, n = # of free blocks
// becomes faster the less fragmented the heap is.
void HeapAllocator::free(HeapPointer* pointer)
{
    // We get the shifted address back, and must look a bit earlier to find out header information.
    MemoryBlock* newFreeBlock = reinterpret_cast<MemoryBlock*>(pointer->rawPtr - sizeof MemoryBlock::header);
    m_freeBlockCount++;

    m_allocatedBytes -= newFreeBlock->header.sizeBytes + sizeof(HeapPointer);
    m_pointers.free(pointer);

    // Check to see if there is a free block following this one.
    MemoryBlock* nextBlock = nullptr;
    MemoryBlock* prevBlock = nullptr;

    { // We want to find the free blocks that surround our newly freed block.
        MemoryBlock* current = m_freeBlocks;
        uintptr_t nextAddr = reinterpret_cast<uintptr_t>(newFreeBlock) + newFreeBlock->header.sizeBytes;

        while (current != nullptr)
        {
            if (reinterpret_cast<uintptr_t>(current) >= nextAddr)
            {
                nextBlock = current;
                break;
            }

            prevBlock = current;
            current = current->freeData.next;
        }
    }

    // Merge surrounding free blocks.
    if (areAdjacent(newFreeBlock, nextBlock))
    {
        merge(newFreeBlock, nextBlock);
    }

    if (areAdjacent(prevBlock, newFreeBlock))
    {
        merge(prevBlock, newFreeBlock);
    }
    else if (prevBlock != nullptr) // We have a non-adjacent previous block.
    {
        prevBlock->freeData.next = newFreeBlock;
    }
    else // We are the first item in the list.
    {
        newFreeBlock->freeData.next = m_freeBlocks;
        m_freeBlocks = newFreeBlock;
    }
}

void HeapAllocator::clear()
{
    m_allocatedBytes = 0;
    m_freeBlockCount = 1;
    m_pointers.clear();

    // Create a new, empty free store right after the pointers.
    m_freeBlocks = new (m_baseAddress + m_pointers.getMaxSizeBytes()) MemoryBlock {};
    m_freeBlocks->header.sizeBytes = m_maxSizeBytes;
    m_freeBlocks->freeData.next = nullptr;
}

// Constant-time operation.
// BEFORE: prev -> free1 -> used -> free2
// AFTER: prev -> used -> free1and2
void HeapAllocator::defragment()
{
    MemoryBlock* free = m_freeBlocks;
    MemoryBlock* next = free->freeData.next;

    if (next != nullptr)
    {
        // Save state before memory shuffles around.
        u8* usedPointer = reinterpret_cast<u8*>(free) + free->header.sizeBytes;
        MemoryBlock* used = reinterpret_cast<MemoryBlock*>(usedPointer);
        u64 freeSize = free->header.sizeBytes;

        // Copy over the data from the used block into the free block.
        memmove(free, used, used->header.sizeBytes);

        // Re-create the moved free block.
        MemoryBlock* newFree = new(reinterpret_cast<u8*>(free) + free->header.sizeBytes) MemoryBlock {};
        newFree->header.sizeBytes = freeSize;

        // Now check if the new free space needs to be merged.
        if (areAdjacent(newFree, next))
        {
            merge(newFree, next);
        }
        else // we are not able to be merged, so repair linked list connections.
        {
            newFree->freeData.next = next;
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

bool HeapAllocator::isFragmented() const
{
    return m_freeBlockCount > 1;
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
        printf("\t%p | offset +%-5llu Free Block [%llu bytes]\n", static_cast<void*>(currentBlock), offsetFromBase,
               currentBlock->header.sizeBytes);
        currentBlock = currentBlock->freeData.next;
    }

    m_pointers.printInfo();
    printf("^^^ heap pointers ^^^\n\n");

    printf("======================\n");
}

// === MEMBER FUNCTIONS ===

// Constant-time operation.
void HeapAllocator::merge(MemoryBlock* first, MemoryBlock* second)
{
    // We should only ever merge consecutive blocks together.
    assert(reinterpret_cast<u8*>(first) + first->header.sizeBytes == reinterpret_cast<u8*>(second));

    first->header.sizeBytes = first->header.sizeBytes + second->header.sizeBytes;
    first->freeData.next = second->freeData.next;
    m_freeBlockCount--;
}

// Constant-time operation.
bool HeapAllocator::areAdjacent(MemoryBlock* first, MemoryBlock* second) // static
{
    if (first == nullptr || second == nullptr)
    {
        return false;
    }

    uintptr_t firstAddr = reinterpret_cast<uintptr_t>(first);
    uintptr_t secondAddr = reinterpret_cast<uintptr_t>(second);
    return firstAddr + first->header.sizeBytes == secondAddr;
}
