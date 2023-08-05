#ifndef VEC2_H
#define VEC2_H
#include <ostream>

#include "vector_base.h"

/**
 * \brief A vector of 2 single-precision floating point values.
 */
struct vec2 : VectorBase<2, f32, vec2>
{
    union { f32 x, data; };
    f32 y;

    // === Lifetime ===
    vec2() = default;
    vec2(f32 x, f32 y);

    // === Constants ===
    static const vec2 ZERO;
    static const vec2 ONE;
    static const vec2 UP;
    static const vec2 DOWN;
    static const vec2 LEFT;
    static const vec2 RIGHT;

    // === Operators ===
    bool operator==(vec2 other) const;
    std::ostream& operator<<(std::ostream& os) const;
};

#endif // VEC2_H
