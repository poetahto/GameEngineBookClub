#include "mat4.h"
#include "math_util.h"

using namespace math;

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
    return ZERO;
}
