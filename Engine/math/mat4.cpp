#include "../string_util.h"
#include "mat4.h"
#include "math_util.h"
#include "vector_base.h"

using namespace math;

std::ostream& operator<<(std::ostream &os, const mat4 &value)
{
    return os << stringFormat(
        "{%f, %f, %f, %f} {%f, %f, %f, %f} {%f, %f, %f, %f} {%f, %f, %f, %f}",
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
            areDifferent |= !nearlyEqual(a[row][column], b[row][column]);
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

mat4 mat4::transpose() const
{
    return mat4
    {
        {
            {data[0][0], data[1][0], data[2][0], data[3][0]},
            {data[0][1], data[1][1], data[2][1], data[3][1]},
            {data[0][2], data[1][2], data[2][2], data[3][2]},
            {data[0][3], data[1][3], data[2][3], data[3][3]},
        }
    };
}

// taken from https://stackoverflow.com/questions/2624422/efficient-4x()4-matrix()-inverse-affine-transform
// before this was really() being annoy()ing.
mat4 mat4::inverse() const
{
    f32 s0 = data[0][0] * data[1][1] - data[1][0] * data[0][1];
    f32 s1 = data[0][0] * data[1][2] - data[1][0] * data[0][2];
    f32 s2 = data[0][0] * data[1][3] - data[1][0] * data[0][3];
    f32 s3 = data[0][1] * data[1][2] - data[1][1] * data[0][2];
    f32 s4 = data[0][1] * data[1][3] - data[1][1] * data[0][3];
    f32 s5 = data[0][2] * data[1][3] - data[1][2] * data[0][3];

    f32 c5 = data[2][2] * data[3][3] - data[3][2] * data[2][3];
    f32 c4 = data[2][1] * data[3][3] - data[3][1] * data[2][3];
    f32 c3 = data[2][1] * data[3][2] - data[3][1] * data[2][2];
    f32 c2 = data[2][0] * data[3][3] - data[3][0] * data[2][3];
    f32 c1 = data[2][0] * data[3][2] - data[3][0] * data[2][2];
    f32 c0 = data[2][0] * data[3][1] - data[3][0] * data[2][1];

    // Should check for 0 determinant
    f32 inverseDeterminant = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    auto b = mat4{};

    b[0][0] = ( data[1][1] * c5 - data[1][2] * c4 + data[1][3] * c3) * inverseDeterminant;
    b[0][1] = (-data[0][1] * c5 + data[0][2] * c4 - data[0][3] * c3) * inverseDeterminant;
    b[0][2] = ( data[3][1] * s5 - data[3][2] * s4 + data[3][3] * s3) * inverseDeterminant;
    b[0][3] = (-data[2][1] * s5 + data[2][2] * s4 - data[2][3] * s3) * inverseDeterminant;

    b[1][0] = (-data[1][0] * c5 + data[1][2] * c2 - data[1][3] * c1) * inverseDeterminant;
    b[1][1] = ( data[0][0] * c5 - data[0][2] * c2 + data[0][3] * c1) * inverseDeterminant;
    b[1][2] = (-data[3][0] * s5 + data[3][2] * s2 - data[3][3] * s1) * inverseDeterminant;
    b[1][3] = ( data[2][0] * s5 - data[2][2] * s2 + data[2][3] * s1) * inverseDeterminant;

    b[2][0] = ( data[1][0] * c4 - data[1][1] * c2 + data[1][3] * c0) * inverseDeterminant;
    b[2][1] = (-data[0][0] * c4 + data[0][1] * c2 - data[0][3] * c0) * inverseDeterminant;
    b[2][2] = ( data[3][0] * s4 - data[3][1] * s2 + data[3][3] * s0) * inverseDeterminant;
    b[2][3] = (-data[2][0] * s4 + data[2][1] * s2 - data[2][3] * s0) * inverseDeterminant;

    b[3][0] = (-data[1][0] * c3 + data[1][1] * c1 - data[1][2] * c0) * inverseDeterminant;
    b[3][1] = ( data[0][0] * c3 - data[0][1] * c1 + data[0][2] * c0) * inverseDeterminant;
    b[3][2] = (-data[3][0] * s3 + data[3][1] * s1 - data[3][2] * s0) * inverseDeterminant;
    b[3][3] = ( data[2][0] * s3 - data[2][1] * s1 + data[2][2] * s0) * inverseDeterminant;

    return b;
}

vec3 mat4::getScale() const
{
    f32 sx = vec3{data[0][0], data[0][1], data[0][2]}.magnitude();
    f32 sy = vec3{data[1][0], data[1][1], data[1][2]}.magnitude();
    f32 sz = vec3{data[2][0], data[2][1], data[2][2]}.magnitude();
    return vec3{sx, sy, sz};
}

vec3 mat4::getTranslation() const
{
    return vec3 { data[3][0], data[3][1], data[3][2] };
}

vec3 mat4::right() const
{
    return vec3 {data[0][0], data[0][1], data[0][2]}.normalized();
}

vec3 mat4::up() const
{
    return vec3 {data[1][0], data[1][1], data[1][2]}.normalized();
}

vec3 mat4::forward() const
{
    return vec3 {data[2][0], data[2][1], data[2][2]}.normalized();
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

mat4 operator*=(mat4& a, const mat4& b)
{
    return a = a * b;
}

// === Atomic Transformations ===
mat4 mat4::translate(vec3 offset)
{
    return translate(offset.x, offset.y, offset.z);
}

mat4 mat4::translate(f32 x, f32 y, f32 z)
{
    return mat4
    {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {x, y, z, 1},
        }
    };
}

mat4 mat4::scale(vec3 scale)
{
    return mat4::scale(scale.x, scale.y, scale.z);
}

mat4 mat4::scale(f32 amount)
{
    return scale(amount, amount, amount);
}

mat4 mat4::scale(f32 x, f32 y, f32 z)
{
    return mat4
    {
        {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
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
