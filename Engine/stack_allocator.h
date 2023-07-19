#ifndef STACK_ALLOCATOR_H
#define STACK_ALLOCATOR_H

#include "memory_util.h"
#include "types.h"


/**
 * \brief An allocator that reserves and frees memory based around the stack data structure.
 * \details You cannot free individual blocks with this allocator. Memory can only be freed
 * to a "Marker", which are placed by the user at runtime.
 */
class StackAllocator
{
public:
    /**
     * \brief A location in the stack, used to free all memory after it was obtained.
     */
    typedef u64 Marker;

    /**
     * \brief Creates a new instance of a stack allocator.
     * \param baseAddress The starting point for storing allocated data.
     * \param maxSizeBytes The upper-limit for allocations.
     */
    void init(void* baseAddress, u64 maxSizeBytes);

    /**
     * \brief Reserves a block of memory for general use.
     * \param sizeBytes The size of the block that should be reserved.
     * \param alignment The alignment of the reserved memory.
     * \return A pointer to the newly reserved block, available for general use.
     */
    void* alloc(u64 sizeBytes, memory::Alignment alignment = memory::DEFAULT_ALIGNMENT);

    /**
     * \brief Marks a location on the stack that can be used to free all following allocations.
     * \return The marker to a location on the stack. Do not edit this value.
     */
    Marker getMarker() const;

    /**
     * \brief Free all allocations following the placement of a marker.
     * \param marker The marker after which all allocations will be freed.
     */
    void freeToMarker(Marker marker);

    /**
     * \brief Free all allocations on the stack.
     */
    void clear();

    // === Byte usage ===
    u64 getMaxSizeBytes() const;
    u64 getRemainingBytes() const;
    u64 getAllocatedBytes() const;

    // === Debugging ===
    void printInfo() const; // TODO: more control over where this outputs, maybe a logger

private:
    u8* m_baseAddress{};
    u64 m_maxSizeBytes{};

    // Most recently allocated block on the stack. Relative to the baseAddress.
    Marker m_topOfStack{};
};

#endif // STACK_ALLOCATOR_H
