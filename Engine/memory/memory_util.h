#ifndef MEMORY_UTIL_H
#define MEMORY_UTIL_H

#include "../types.h"

namespace memory
{
    /**
     * \brief An amount of alignment that memory should be aligned to.
     */
    typedef u16 Alignment;

    /**
     * \brief The alignment value assumed by most functions, by default.
     */
    constexpr Alignment DEFAULT_ALIGNMENT = 8;
    // todo: is it better to force explicit alignment for everything?

    /**
     * \brief Checks a pointer to see if it is aligned to a certain value.
     * \param pointer The pointer to check for alignment.
     * \param align The alignment value to be checked for.
     * \return True if the pointer is aligned, false otherwise.
     */
    bool isAligned(const void* pointer, Alignment align);

    /**
     * \brief Determines the amount of bytes needed to align some data.
     * \param sizeBytes The original size you want to allocate.
     * \param align The alignment value you want to apply.
     * \return The total size needed to align the original data.
     */
    u64 getAlignedSize(u64 sizeBytes, Alignment align = DEFAULT_ALIGNMENT);

    /**
     * \brief Shifts a pointer such that it respects some alignment.
     * \param basePointer The pointer that should be aligned.
     * \param align The alignment to apply to the pointer.
     * \return The newly aligned version of basePointer.
     * \details You may want to use "memory::getAlignedSize()" to allocate the pointer
     * that is passed in to this function.
     */
    u8* alignPointer(u8* basePointer, Alignment align = DEFAULT_ALIGNMENT);

} // namespace memory

// This is templated so its easy to change the strategy for getting type alignment.
/**
 * \brief Calculates the ideal alignment for a given type.
 * \tparam T The type that should be aligned for.
 * \return An alignment value appropriate for the given type.
 */
template <class T>
memory::Alignment align()
{
    return alignof(T);
}

#endif // MEMORY_UTIL_H
