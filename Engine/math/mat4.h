#ifndef MATRIX_H
#define MATRIX_H

#include "../types.h"
#include "vec3.h"

struct mat4
{
    f32 data[4][4];
    vec3 transformPoint(vec3 point) const;
    vec3 transformDirection(vec3 direction) const;

    const f32* operator[](s32 index) const;
    f32* operator[](s32 index);

    // === Atomic Transformations ===
    static mat4 translate(vec3 offset);
    static mat4 scale(vec3 scale);
    static mat4 rotateX(f32 amount);
    static mat4 rotateY(f32 amount);
    static mat4 rotateZ(f32 amount);

    // === Constants ===
    static const mat4 IDENTITY;
    static const mat4 ZERO;
};

// === Operators ===
bool operator==(const mat4& a, const mat4& b);
bool operator!=(const mat4& a, const mat4& b);
mat4 operator*(const mat4& first, const mat4& second);
std::ostream& operator<<(std::ostream& os, const mat4& value);

#endif // MATRIX_H
