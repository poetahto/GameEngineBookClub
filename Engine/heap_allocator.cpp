#include <cstdio>
#include <cstring>
#include <new>
#include "heap_allocator.h"

using namespace memory;

void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_baseAddress = static_cast<u8*>(baseAddress);
    m_maxAllocations = maxSizeBytes / (sizeof(MemoryBlock) + sizeof(HeapPointer) + 1);

    // Set aside memory for non-relocatable pointers that can be adjusted when defragmentation.
    u64 pointersSize = m_maxAllocations * sizeof(HeapPointer);
    m_maxSizeBytes = maxSizeBytes - pointersSize;
    m_pointers.init(baseAddress, pointersSize, sizeof(HeapPointer));

    // Start off with a clean heap.
    clear();
}

// worst-case O(n) time operation, best-case constant-time operation, n = # of free blocks
// becomes faster the less fragmented the heap is.
HeapPointer* HeapAllocator::alloc(u64 sizeBytes, Alignment alignment)
{
    // we only want allocated in chunks the size of our free block - this way we can always have room for free storage
    u64 baseSize = sizeBytes + alignment + sizeof(u64); // the u64 is our header information
    u64 trueSizeBytes = static_cast<u64>(ceilf(static_cast<f32>(baseSize) / sizeof(MemoryBlock))) * sizeof(MemoryBlock);
    m_allocatedBytes += trueSizeBytes + sizeof(HeapPointer);

    // find the block to return for our users
    MemoryBlock* freeBlock = m_freeBlocks;
    MemoryBlock* prevBlock = nullptr;

    while (freeBlock != nullptr)
    {
        if (freeBlock->sizeBytes >= trueSizeBytes)
        {
            break;
        }

        prevBlock = freeBlock;
        freeBlock = freeBlock->next;
    }

    // We failed to find a free block that's big enough.
    if (freeBlock == nullptr)
    {
        // todo: log out-of-memory error (or too fragmented!)
        return nullptr;
    }

    u64 leftoverBytes = freeBlock->sizeBytes - trueSizeBytes;
    freeBlock->sizeBytes = trueSizeBytes;
    m_freeBlockCount--;
    u8* freeBlockDataPointer = reinterpret_cast<u8*>(&freeBlock->next);

    // Check if we need to insert an empty block after the used one.
    if (leftoverBytes > 0)
    {
        void* newFreePointer = reinterpret_cast<u8*>(freeBlock) + trueSizeBytes;
        MemoryBlock* newFreeBlock = new (newFreePointer) MemoryBlock {};
        newFreeBlock->sizeBytes = leftoverBytes;
        newFreeBlock->next = freeBlock->next;
        m_freeBlockCount++;

        freeBlock->next = newFreeBlock;
    }

    // Make sure the linked list remains valid.
    if (prevBlock != nullptr)
    {
        prevBlock->next = freeBlock->next;
    }
    else // We have no previous element, hence we are the first in the list.
    {
        m_freeBlocks = freeBlock->next;
    }

    // Align our data before returning it (taken from "Game Engine Architecture 3rd" 6.2.1.3)
    u8* alignedPointer = alignPointer(freeBlockDataPointer, alignment);

    if (alignedPointer == freeBlockDataPointer)
    {
        alignedPointer += alignment;
    }

    ptrdiff_t shift = alignedPointer - freeBlockDataPointer;
    alignedPointer[-1] = static_cast<u8>(shift & 0xFF);

    return new (m_pointers.alloc()) HeapPointer { alignedPointer };
}

// worst-case O(n) time operation, best-case constant-time operation, n = # of free blocks
// becomes faster the less fragmented the heap is.
void HeapAllocator::free(HeapPointer* pointer)
{
    // Find base address, from aligned return value.
    ptrdiff_t shift = pointer->rawPtr[-1];

    if (shift == 0)
    {
        shift = 256;
    }

    u8* basePointer = pointer->rawPtr - shift;

    // We get the shifted address back, and must look a bit earlier to find out header information.
    MemoryBlock* newFreeBlock = reinterpret_cast<MemoryBlock*>(basePointer - sizeof(u64));
    m_freeBlockCount++;

    m_allocatedBytes -= newFreeBlock->sizeBytes + sizeof(HeapPointer);
    m_pointers.free(pointer);

    // Check to see if there is a free block following this one.
    MemoryBlock* nextBlock = nullptr;
    MemoryBlock* prevBlock = nullptr;

    { // We want to find the free blocks that surround our newly freed block.
        MemoryBlock* current = m_freeBlocks;
        uintptr_t nextAddr = reinterpret_cast<uintptr_t>(newFreeBlock) + newFreeBlock->sizeBytes;

        while (current != nullptr)
        {
            if (reinterpret_cast<uintptr_t>(current) >= nextAddr)
            {
                nextBlock = current;
                break;
            }

            prevBlock = current;
            current = current->next;
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
        prevBlock->next = newFreeBlock;
    }
    else // We are the first item in the list.
    {
        newFreeBlock->next = m_freeBlocks;
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
    m_freeBlocks->sizeBytes = m_maxSizeBytes;
    m_freeBlocks->next = nullptr;
}

// Constant-time operation.
// BEFORE: prev -> free1 -> used -> free2
// AFTER: prev -> used -> free1and2
void HeapAllocator::defragment()
{
    MemoryBlock* free = m_freeBlocks;
    MemoryBlock* next = free->next;

    if (next != nullptr)
    {
        // Save state before memory shuffles around.
        u8* usedPointer = reinterpret_cast<u8*>(free) + free->sizeBytes;
        MemoryBlock* used = reinterpret_cast<MemoryBlock*>(usedPointer);
        u64 freeSize = free->sizeBytes;

        // Copy over the data from the used block into the free block.
        memmove(free, used, used->sizeBytes);

        // Re-create the moved free block.
        MemoryBlock* newFree = new(reinterpret_cast<u8*>(free) + free->sizeBytes) MemoryBlock {};
        newFree->sizeBytes = freeSize;

        // Now check if the new free space needs to be merged.
        if (areAdjacent(newFree, next))
        {
            merge(newFree, next);
        }
        else // we are not able to be merged, so repair linked list connections.
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
        printf("\t%p | offset +%-5llu Free Block [%llu bytes]\n", static_cast<void*>(currentBlock), offsetFromBase,currentBlock->sizeBytes);
        currentBlock = currentBlock->next;
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
    assert(reinterpret_cast<u8*>(first) + first->sizeBytes == reinterpret_cast<u8*>(second));

    first->sizeBytes = first->sizeBytes + second->sizeBytes;
    first->next = second->next;
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
    return firstAddr + first->sizeBytes == secondAddr;
}
