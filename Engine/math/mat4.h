#ifndef MATRIX_H
#define MATRIX_H

#include "../types.h"
#include "vec3.h"

struct Mat4
{
    f32 data[4][4];
    Vec3 transformPoint(Vec3 point) const;
    Vec3 transformDirection(Vec3 direction) const;
    Mat4 transpose() const;

    // === Decomposition ===
    Mat4 inverse() const;
    Vec3 getScale() const;
    Vec3 getTranslation() const;
    // todo: get rotation quaternion when done

    // === Basis ===
    Vec3 right() const;
    Vec3 up() const;
    Vec3 forward() const;

    const f32* operator[](s32 index) const;
    f32* operator[](s32 index);

    // === Atomic Transformations ===
    static Mat4 translate(Vec3 offset);
    static Mat4 translate(f32 x, f32 y, f32 z);
    static Mat4 scale(Vec3 scale);
    static Mat4 scale(f32 x, f32 y, f32 z);
    static Mat4 scale(f32 amount);
    static Mat4 rotateX(f32 amount);
    static Mat4 rotateY(f32 amount);
    static Mat4 rotateZ(f32 amount);
    static Mat4 perspective(f32 near, f32 far, s32 screenWidth, s32 screenHeight, f32 fov);
    static Mat4 orthographic(f32 near, f32 far, s32 screenWidth, s32 screenHeight, f32 size);
    static Mat4 orthographic(f32 near, f32 far, f32 top, f32 bottom, f32 left, f32 right);
    // todo: more ways to define rotations (probably from quaternion, and really overload them)

    // === Constants ===
    static const Mat4 IDENTITY;
    static const Mat4 ZERO;
};

// === Operators ===
std::ostream& operator<<(std::ostream& os, const Mat4& value);
bool operator==(const Mat4& a, const Mat4& b);
bool operator!=(const Mat4& a, const Mat4& b);
Mat4 operator*(const Mat4& first, const Mat4& second);
Mat4 operator *=(Mat4& a, const Mat4& b);

#endif // MATRIX_H
