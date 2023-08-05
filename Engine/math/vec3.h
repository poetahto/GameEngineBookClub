#ifndef VEC3_H
#define VEC3_H

#include <ostream>
#include "vector_base.h"

/**
 * \brief A vector of 3 single-precision floating point values.
 */
struct vec3 : VectorBase<3, f32, vec3>
{
    union { f32 x, r, data; };
    union { f32 y, g; };
    union { f32 z, b; };

    // === Lifetime ===
    vec3() = default;
    vec3(f32 x, f32 y, f32 z);

    // === Operations ===
    static vec3 cross(vec3 first, vec3 second);

    // === Constants ===
    static const vec3 ZERO;
    static const vec3 ONE;
    static const vec3 UP;
    static const vec3 DOWN;
    static const vec3 LEFT;
    static const vec3 RIGHT;
    static const vec3 FORWARD;
    static const vec3 BACK;

    // === Operators ===
    bool operator==(vec3 other) const;
    std::ostream& operator<<(std::ostream& os) const;
};

#endif // VEC3_H
