#ifndef STRING_NAME_HPP
#define STRING_NAME_HPP

#include "types.hpp"

/**
 * \brief The datatype used beneath the hashing algorithm.
 * Bigger range = less likely collisions.
 */
typedef u64 Hash;

/**
 * \brief A hashed version of a string.
 * During debugging builds, you can still see the
 * un-hashed value.
 */
struct StringName
{
    /**
     * \brief The hashed equivalent for a given string ID.
     */
    Hash hash;

private:
#if _DEBUG
    /**
     * \brief This parameter is only used for debugging purposes,
     * and is stripped out during release builds.
     */
    const char* name;
    friend StringName operator""_sn(const char*);
#endif // _DEBUG
};

/**
 * \brief Creates a hashed version of a given C-string at compile time.
 * \param rawName The string that should be hashed.
 * \return A hashed identifier for the string.
 */
inline StringName operator""_sn(const char* rawName)
{
    StringName result;

#if _DEBUG
    result.name = rawName;
#endif // _DEBUG

    // Perform the FNV-1a hash on the name, targeting 64 bits.
    {
        Hash hash = 14695981039346656037u;

        while (*rawName != 0)
        {
            hash ^= *rawName++;
            hash *= 1099511628211u;
        }

        result.hash = hash;
    }

    return result;
}


#endif // STRING_NAME_HPP
