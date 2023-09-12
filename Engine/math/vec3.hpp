#ifndef VEC3_H
#define VEC3_H

#include <ostream>
#include "vector_base.h"

/**
 * \brief A vector of 3 single-precision floating point values.
 */
struct Vec3 : VectorBase<3, f32, Vec3>
{
    union { f32 x, r, data; };
    union { f32 y, g; };
    union { f32 z, b; };

    // === Lifetime ===
    Vec3() = default;
    Vec3(f32 x, f32 y, f32 z);

    // === Operations ===
    static Vec3 cross(Vec3 first, Vec3 second);

    // === Constants ===
    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 LEFT;
    static const Vec3 RIGHT;
    static const Vec3 FORWARD;
    static const Vec3 BACK;

    // === Operators ===
    bool operator==(Vec3 other) const;
    std::ostream& operator<<(std::ostream& os) const;
};

#endif // VEC3_H
