#ifndef VEC2_H
#define VEC2_H
#include <ostream>

#include "vector_base.h"

/**
 * \brief A vector of 2 single-precision floating point values.
 */
struct Vec2 : VectorBase<2, f32, Vec2>
{
    union { f32 x, data; };
    f32 y;

    // === Lifetime ===
    Vec2() = default;
    Vec2(f32 x, f32 y);

    // === Constants ===
    static const Vec2 ZERO;
    static const Vec2 ONE;
    static const Vec2 UP;
    static const Vec2 DOWN;
    static const Vec2 LEFT;
    static const Vec2 RIGHT;

    // === Operators ===
    bool operator==(Vec2 other) const;
    std::ostream& operator<<(std::ostream& os) const;
};

#endif // VEC2_H
