#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include "../types.h"
#include "vec3.h"

struct mat4
{
    f32 data[4][4];
    vec3 transformPoint(vec3 point) const;
    vec3 transformDirection(vec3 direction) const;
    mat4 transpose() const;

    // === Decomposition ===
    mat4 inverse() const;
    vec3 getScale() const;
    vec3 getTranslation() const;
    // todo: get rotation quaternion when done

    // === Basis ===
    vec3 right() const;
    vec3 up() const;
    vec3 forward() const;

    const f32* operator[](s32 index) const;
    f32* operator[](s32 index);

    // === Atomic Transformations ===
    static mat4 translate(vec3 offset);
    static mat4 translate(f32 x, f32 y, f32 z);
    static mat4 scale(vec3 scale);
    static mat4 scale(f32 x, f32 y, f32 z);
    static mat4 scale(f32 amount);
    static mat4 rotateX(f32 amount);
    static mat4 rotateY(f32 amount);
    static mat4 rotateZ(f32 amount);
    // todo: more ways to define rotations (probably from quaternion, and really overload them)
    // todo: projection matrix when doing rendering

    // === Constants ===
    static const mat4 IDENTITY;
    static const mat4 ZERO;
};

// === Operators ===
std::ostream& operator<<(std::ostream& os, const mat4& value);
bool operator==(const mat4& a, const mat4& b);
bool operator!=(const mat4& a, const mat4& b);
mat4 operator*(const mat4& first, const mat4& second);
mat4 operator *=(mat4& a, const mat4& b);

#endif // MATRIX_H
