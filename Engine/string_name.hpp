#ifndef STRING_NAME_HPP
#define STRING_NAME_HPP

#include "types.hpp"
#include <unordered_map>

/**
 * \brief A hashed version of a string.
 * During debugging builds, you can still see the
 * un-hashed value.
 */
struct StringName
{
    /**
     * \brief The datatype used beneath the hashing algorithm.
     * Bigger range = less likely collisions.
     */
    typedef u64 Hash;

    explicit StringName(Hash stringHash) : hash {stringHash}
    {
    }

    /**
     * \brief The hashed equivalent for a given string ID.
     */
    const Hash hash;

    /**
     * \brief Creates a hashed version of the given C-string.
     * \param string The string that should be hashed.
     * \return A hashed identified for the string.
     */
    static Hash createHash(const char* string)
    {
        // Perform the FNV-1a hash on the name, targeting 64 bits.
        Hash hash = 14695981039346656037u;

        while (*string != 0)
        {
            hash ^= *string++;
            hash *= 1099511628211u;
        }

        return hash;
    }

    bool operator ==(const StringName& other) const
    {
        return hash == other.hash;
    }

private:
#if _DEBUG
    /**
     * \brief This parameter is only used for debugging purposes,
     * and is stripped out during release builds.
     */
    const char* name {};
#endif // _DEBUG

    friend StringName operator ""_sn(const char*, size_t);
};

// Enable StringNames to be hashed and used in STL data structures.
template<>
struct std::hash<StringName>
{
    std::size_t operator()(const StringName& stringName) const noexcept
    {
        return hash<unsigned long long>()(stringName.hash);
    }
};

/**
 * \brief Creates a hashed version of a given C-string at compile time.
 * \param rawName The string that should be hashed.
 * \return A hashed identifier for the string.
 */
StringName operator ""_sn(const char* rawName, size_t);

#endif // STRING_NAME_HPP
