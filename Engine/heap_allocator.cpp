#include <cstdio>
#include <new>
#include <cassert>
#include <cstring>
#include "heap_allocator.h"

void HeapAllocator::init(void* baseAddress, u64 maxSizeBytes)
{
    m_maxSizeBytes = maxSizeBytes;
    m_firstHeapPointer = new (baseAddress) HeapPointer{ nullptr };

    u64 maxAllocations = maxSizeBytes / (sizeof(MemoryBlock) + sizeof(HeapPointer));

    m_heapPointers.init(m_firstHeapPointer, maxAllocations * sizeof(HeapPointer));

    // Our first block begins after our fixed header for HeapPointers.
    m_firstBlock = new (m_firstHeapPointer + maxAllocations) MemoryBlock;

    // Start off with a clean heap.
    clear();
}

HeapPointer* HeapAllocator::alloc(u64 sizeBytes)
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

    return new (m_heapPointers.alloc()) HeapPointer { newUsedBuffer + sizeof(MemoryBlock) };
}

void HeapAllocator::free(HeapPointer* heapPointer)
{
    assert(heapPointer != nullptr);

    u8* newFreePointer = static_cast<u8*>(heapPointer->getPointer()) - sizeof(MemoryBlock);
    MemoryBlock* newFreeBlock = reinterpret_cast<MemoryBlock*>(newFreePointer);
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

    m_heapPointers.free(heapPointer);
}

void HeapAllocator::clear()
{
    m_firstBlock->isFree = true;
    m_firstBlock->next = nullptr;
    m_firstBlock->previous = nullptr;
    m_firstBlock->sizeBytes = m_maxSizeBytes;

    m_heapPointers.clear();
    m_allocatedBytes = 0;
}

void HeapAllocator::defragment()
{
    MemoryBlock* oldFreeBlock = getFreeBlock();
    MemoryBlock* usedBlock = oldFreeBlock->next;

    if (usedBlock != nullptr)
    {
        // Saving important state before we start shuffling stuff
        u64 oldFreeSize = oldFreeBlock->sizeBytes;
        MemoryBlock* next = usedBlock->next;
        MemoryBlock* previous = oldFreeBlock->previous;
        u8* baseAddress = reinterpret_cast<u8*>(oldFreeBlock);
        u8* newFreeAddress = baseAddress + usedBlock->sizeBytes;

        memmove(baseAddress, usedBlock, usedBlock->sizeBytes);

        MemoryBlock* newFreeBlock = new (newFreeAddress) MemoryBlock
        {
            true,
            oldFreeSize,
            next,
            oldFreeBlock
        };

        oldFreeBlock->previous = previous;
        oldFreeBlock->next = newFreeBlock;

        // Now check if the new free space needs to be merged.
        MemoryBlock* potentiallyFreeBlock = newFreeBlock->next;

        if (potentiallyFreeBlock != nullptr && potentiallyFreeBlock->isFree)
        {
            merge(newFreeBlock, potentiallyFreeBlock);
        }

        // Now scan the HeapPointer list, and update the one pointing to the block that just got moved.
        u64 maxAllocations = m_maxSizeBytes / (sizeof(MemoryBlock) + sizeof(HeapPointer));

        for (u64 i = 0; i < maxAllocations; i++)
        {
            if (m_firstHeapPointer[i].m_basePointer == usedBlock)
            {
                m_firstHeapPointer[i].m_basePointer = oldFreeBlock;
                return;
                // I _think_ its impossible for us to have multiple pointers in our store, so early out.
            }
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

    MemoryBlock* currentBlock = m_firstBlock;

    while (currentBlock != nullptr)
    {
        u64 offsetFromBase = reinterpret_cast<uintptr_t>(currentBlock) - reinterpret_cast<uintptr_t>(m_firstBlock);
        printf("\t%p | offset +%-5llu %5s Block [%llu bytes]\n", static_cast<void*>(currentBlock), offsetFromBase, currentBlock->isFree ? "Free" : "Used", currentBlock->sizeBytes);
        currentBlock = currentBlock->next;
    }

    m_heapPointers.printInfo();

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

// === HEAP POINTER ===

HeapPointer::HeapPointer(void* basePointer) : m_basePointer {basePointer}
{
}

void* HeapPointer::getPointer() const
{
    return m_basePointer;
}
