#ifndef MATRIX_H
#define MATRIX_H

#include "../types.h"

struct mat4
{
    f32 data[4][4];

    const f32* operator[](s32 index) const;
    f32* operator[](s32 index);
};

// === Operators ===
bool operator==(const mat4& a, const mat4& b);
bool operator!=(const mat4& a, const mat4& b);
mat4 operator*(const mat4& first, const mat4& second);

// === Constants ===
constexpr mat4 IDENTITY
{
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }
};

constexpr mat4 ZERO
{
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    }
};

#endif // MATRIX_H
