#include "mat4.h"
#include "math_util.h"

using namespace math;

/*
 * Note: I have 1 more week to finish up matrix stuff.
 * Stuff that still needs implementing:
 * - human-readable printing for mat and vec
 * - Transpose
 * - Translation Matrix
 * - Rotation Matrix
 * - Scale Matrix
 * - multiplying with vectors
 * - extracting basis vectors?
 * - maybe inverse?
 */

bool operator==(const mat4& a, const mat4& b)
{
    bool areDifferent = false;

    for (u8f row = 0; row < 4; row++)
    {
        for (u8f column = 0; column < 4; column++)
        {
            areDifferent |= !nearlyEqual(a[row][column], b[row][column]);
        }
    }

    return !areDifferent;
}

bool operator!=(const mat4& a, const mat4& b)
{
    return !(a == b);
}

const f32* mat4::operator[](s32 index) const
{
    return data[index];
}

f32* mat4::operator[](s32 index)
{
    return data[index];
}

mat4 operator*(const mat4& first, const mat4& second)
{
    mat4 result {};

    for (u8f row = 0; row < 4; row++)
    {
        for (u8f column = 0; column < 4; column++)
        {
            // Dot product the rows and columns between the two matrices.
            result[row][column] =
                first[row][0] * second[0][column] +
                first[row][1] * second[1][column] +
                first[row][2] * second[2][column] +
                first[row][3] * second[3][column];
        }
    }

    return result;
}
