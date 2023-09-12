#ifndef VEC4_H
#define VEC4_H

#include <ostream>
#include "vector_base.h"

struct Vec3;

struct Vec4 : VectorBase<4, f32, Vec4>
{
    union { f32 x, r, data; };
    union {f32 y, g; };
    union {f32 z, b; };
    union {f32 w, a; };

    // === Lifetime ===
    Vec4() = default;
    Vec4(f32 x, f32 y, f32 z, f32 w);

    // === Operators ===
    bool operator==(Vec4 other) const;
    std::ostream& operator<<(std::ostream& os) const;
};

#endif // VEC4_H
