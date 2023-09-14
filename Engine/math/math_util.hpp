#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include "../types.hpp"

namespace Math
{
    /**
     * \brief Compares two floating point values to see if they are nearly equal.
     * \param a The first floating point value.
     * \param b The second floating point value.
     * \return True if the values are nearly equal, false otherwise.
     */
    bool nearlyEqual(f32 a, f32 b);

    constexpr f32 PI { 3.14159265358979323846f };

    // ReSharper disable once CppInconsistentNaming
    constexpr f32 DEG2RAD { PI / 180 };
    // ReSharper disable once CppInconsistentNaming
    constexpr f32 RAD2DEG { 180 / PI };

} // namespace math

#endif // MATH_UTIL_H
