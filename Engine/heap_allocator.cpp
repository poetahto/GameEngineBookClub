#include <cstdio>
#include <new>
#include <cassert>
#include "heap_allocator.h"

void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_maxSizeBytes = maxSizeBytes;
    m_firstBlock = new (baseAddress) MemoryBlock;

    // Start off with a clean heap.
    clear();
}

void* HeapAllocator::alloc(u64 sizeBytes)
{
    u64 fullSizeBytes = sizeBytes + sizeof(MemoryBlock);
    m_allocatedBytes += fullSizeBytes;
    MemoryBlock* oldFreeBlock = getFreeBlock(fullSizeBytes);
    MemoryBlock* oldNext = oldFreeBlock->next;
    MemoryBlock* oldPrevious = oldFreeBlock->previous;

    if (oldFreeBlock == nullptr)
    {
        // todo: log out-of-memory error (or too fragmented!)
        return nullptr;
    }

    u64 leftoverBytes = oldFreeBlock->sizeBytes - fullSizeBytes;

    u8* newUsedBuffer = reinterpret_cast<u8*>(oldFreeBlock);
    u8* newFreeBuffer = newUsedBuffer + fullSizeBytes;

    MemoryBlock* newUsedBlock = new (newUsedBuffer) MemoryBlock { false, fullSizeBytes };
    MemoryBlock* newFreeBlock = new (newFreeBuffer) MemoryBlock { true, leftoverBytes };

    newUsedBlock->previous = oldPrevious;
    newUsedBlock->next = newFreeBlock;

    newFreeBlock->previous = newUsedBlock;
    newFreeBlock->next = oldNext;

    return newUsedBlock;
}

void HeapAllocator::free(void* buffer)
{
    assert(buffer != nullptr);

    MemoryBlock* newFreeBlock = static_cast<MemoryBlock*>(buffer);
    newFreeBlock->isFree = true;
    m_allocatedBytes -= newFreeBlock->sizeBytes;

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
    m_firstBlock->isFree = true;
    m_firstBlock->next = nullptr;
    m_firstBlock->previous = nullptr;
    m_firstBlock->sizeBytes = m_maxSizeBytes;

    m_allocatedBytes = 0;
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

    MemoryBlock* currentBlock = m_firstBlock;

    while (currentBlock != nullptr)
    {
        printf("\tBlock %p [size %llu]\n", static_cast<void*>(currentBlock), currentBlock->sizeBytes);
        currentBlock = currentBlock->next;
    }

    printf("======================\n");
}

// === MEMBER FUNCTIONS ===

HeapAllocator::MemoryBlock* HeapAllocator::getFreeBlock(u64 sizeBytes) const
{
    MemoryBlock* currentBlock = m_firstBlock;

    while (currentBlock != nullptr)
    {
        if (currentBlock->isFree && currentBlock->sizeBytes >= sizeBytes)
        {
            return currentBlock;
        }

        currentBlock = currentBlock->next;
    }

    return nullptr;
}

void HeapAllocator::merge(MemoryBlock* first, MemoryBlock* second) // Static
{
    u64 totalSizeBytes = first->sizeBytes + second->sizeBytes;
    MemoryBlock* next = second->next;
    MemoryBlock* previous = first->previous;

    new (first) MemoryBlock
    {
        true,
        totalSizeBytes,
        next,
        previous
    };
}
