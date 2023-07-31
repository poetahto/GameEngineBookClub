#include <cmath>
#include "../string_util.h"
#include "mat4.h"
#include "math_util.h"

using namespace math;

/*
 * Note: I have 1 more week to finish up matrix stuff.
 * Stuff that still needs implementing:
 * - Transpose
 * - extracting basis vectors?
 * - maybe inverse?
 */

std::ostream& operator<<(std::ostream &os, const mat4 &value)
{
    return os << stringFormat(
        "{%.1f, %.1f, %.1f, %.1f} {%.1f, %.1f, %.1f, %.1f} {%.1f, %.1f, %.1f, %.1f} {%.1f, %.1f, %.1f, %.1f}",
                        value.data[0][0], value.data[0][1], value.data[0][2], value.data[0][3],
                        value.data[1][0], value.data[1][1], value.data[1][2], value.data[1][3],
                        value.data[2][0], value.data[2][1], value.data[2][2], value.data[2][3],
                        value.data[3][0], value.data[3][1], value.data[3][2], value.data[3][3]);
}

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

vec3 mat4::transformPoint(vec3 point) const
{
    return vec3
    {
        data[0][0] * point.x + data[1][0] * point.y + data[2][0] * point.z + data[3][0],
        data[0][1] * point.x + data[1][1] * point.y + data[2][1] * point.z + data[3][1],
        data[0][2] * point.x + data[1][2] * point.y + data[2][2] * point.z + data[3][2],
    };
}

vec3 mat4::transformDirection(vec3 direction) const
{
    return vec3
    {
        data[0][0] * direction.x + data[1][0] * direction.y + data[2][0] * direction.z,
        data[0][1] * direction.x + data[1][1] * direction.y + data[2][1] * direction.z,
        data[0][2] * direction.x + data[1][2] * direction.y + data[2][2] * direction.z,
    };
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
    mat4 result{};

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

// === Atomic Transformations ===
mat4 mat4::translate(vec3 offset)
{
    return mat4
    {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {offset.x, offset.y, offset.z, 1},
        }
    };
}

mat4 mat4::scale(vec3 scale)
{
    return mat4
    {
        {
            {scale.x, 0, 0, 0},
            {0, scale.y, 0, 0},
            {0, 0, scale.z, 0},
            {0, 0, 0, 1},
        }
    };
}

mat4 mat4::rotateX(f32 amount)
{
    return mat4
    {
        {
            {1, 0, 0, 0},
            {0, cos(amount), sin(amount), 0},
            {0, -sin(amount), cos(amount), 0},
            {0, 0, 0, 1},
        }
    };
}

mat4 mat4::rotateY(f32 amount)
{
    return mat4
    {
        {
            {cos(amount), 0, -sin(amount), 0},
            {0, 1, 0, 0},
            {sin(amount), 0, cos(amount), 0},
            {0, 0, 0, 1},
        }
    };
}

mat4 mat4::rotateZ(f32 amount)
{
    return mat4
    {
        {
            {cos(amount), sin(amount), 0, 0},
            {-sin(amount), cos(amount), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    };
}

// === Constants ===
const mat4 mat4::IDENTITY
{
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }
};

const mat4 mat4::ZERO
{
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    }
};
