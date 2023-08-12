#include "../string_util.h"
#include "mat4.h"

#include <complex>

#include "math_util.h"
#include "vector_base.h"

using namespace Math;

std::ostream& operator<<(std::ostream& os, const Mat4& value)
{
    return os << stringFormat(
        "{%f, %f, %f, %f} {%f, %f, %f, %f} {%f, %f, %f, %f} {%f, %f, %f, %f}",
        value.data[0][0], value.data[0][1], value.data[0][2], value.data[0][3],
        value.data[1][0], value.data[1][1], value.data[1][2], value.data[1][3],
        value.data[2][0], value.data[2][1], value.data[2][2], value.data[2][3],
        value.data[3][0], value.data[3][1], value.data[3][2], value.data[3][3]);
}

bool operator==(const Mat4& a, const Mat4& b)
{
    bool areDifferent = false;

    for (u8f row = 0; row < 4; row++)
    {
        for (u8f column = 0; column < 4; column++)
            areDifferent |= !nearlyEqual(a[row][column], b[row][column]);
    }

    return !areDifferent;
}

bool operator!=(const Mat4& a, const Mat4& b)
{
    return !(a == b);
}

Vec3 Mat4::transformPoint(Vec3 point) const
{
    return Vec3
    {
        data[0][0] * point.x + data[1][0] * point.y + data[2][0] * point.z + data[3][0],
        data[0][1] * point.x + data[1][1] * point.y + data[2][1] * point.z + data[3][1],
        data[0][2] * point.x + data[1][2] * point.y + data[2][2] * point.z + data[3][2],
    };
}

Vec3 Mat4::transformDirection(Vec3 direction) const
{
    direction.z *= -1;

    return Vec3
    {
        data[0][0] * direction.x + data[1][0] * direction.y + data[2][0] * direction.z,
        data[0][1] * direction.x + data[1][1] * direction.y + data[2][1] * direction.z,
        data[0][2] * direction.x + data[1][2] * direction.y + data[2][2] * direction.z,
    };
}

Mat4 Mat4::transpose() const
{
    return Mat4
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
Mat4 Mat4::inverse() const
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

    auto b = Mat4{};

    b[0][0] = (data[1][1] * c5 - data[1][2] * c4 + data[1][3] * c3) * inverseDeterminant;
    b[0][1] = (-data[0][1] * c5 + data[0][2] * c4 - data[0][3] * c3) * inverseDeterminant;
    b[0][2] = (data[3][1] * s5 - data[3][2] * s4 + data[3][3] * s3) * inverseDeterminant;
    b[0][3] = (-data[2][1] * s5 + data[2][2] * s4 - data[2][3] * s3) * inverseDeterminant;

    b[1][0] = (-data[1][0] * c5 + data[1][2] * c2 - data[1][3] * c1) * inverseDeterminant;
    b[1][1] = (data[0][0] * c5 - data[0][2] * c2 + data[0][3] * c1) * inverseDeterminant;
    b[1][2] = (-data[3][0] * s5 + data[3][2] * s2 - data[3][3] * s1) * inverseDeterminant;
    b[1][3] = (data[2][0] * s5 - data[2][2] * s2 + data[2][3] * s1) * inverseDeterminant;

    b[2][0] = (data[1][0] * c4 - data[1][1] * c2 + data[1][3] * c0) * inverseDeterminant;
    b[2][1] = (-data[0][0] * c4 + data[0][1] * c2 - data[0][3] * c0) * inverseDeterminant;
    b[2][2] = (data[3][0] * s4 - data[3][1] * s2 + data[3][3] * s0) * inverseDeterminant;
    b[2][3] = (-data[2][0] * s4 + data[2][1] * s2 - data[2][3] * s0) * inverseDeterminant;

    b[3][0] = (-data[1][0] * c3 + data[1][1] * c1 - data[1][2] * c0) * inverseDeterminant;
    b[3][1] = (data[0][0] * c3 - data[0][1] * c1 + data[0][2] * c0) * inverseDeterminant;
    b[3][2] = (-data[3][0] * s3 + data[3][1] * s1 - data[3][2] * s0) * inverseDeterminant;
    b[3][3] = (data[2][0] * s3 - data[2][1] * s1 + data[2][2] * s0) * inverseDeterminant;

    return b;
}

Vec3 Mat4::getScale() const
{
    f32 sx = Vec3{data[0][0], data[0][1], data[0][2]}.magnitude();
    f32 sy = Vec3{data[1][0], data[1][1], data[1][2]}.magnitude();
    f32 sz = Vec3{data[2][0], data[2][1], data[2][2]}.magnitude();
    return Vec3{sx, sy, sz};
}

Vec3 Mat4::getTranslation() const
{
    return Vec3{data[3][0], data[3][1], data[3][2]};
}

Vec3 Mat4::right() const
{
    return Vec3{data[0][0], data[0][1], data[0][2]}.normalized();
}

Vec3 Mat4::up() const
{
    return Vec3{data[1][0], data[1][1], data[1][2]}.normalized();
}

Vec3 Mat4::forward() const
{
    return Vec3{data[2][0], data[2][1], data[2][2]}.normalized();
}

const f32* Mat4::operator[](s32 index) const
{
    return data[index];
}

f32* Mat4::operator[](s32 index)
{
    return data[index];
}

Mat4 operator*(const Mat4& first, const Mat4& second)
{
    Mat4 result{};

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

Mat4 operator*=(Mat4& a, const Mat4& b)
{
    return a = a * b;
}

// === Atomic Transformations ===
Mat4 Mat4::translate(Vec3 offset)
{
    return translate(offset.x, offset.y, offset.z);
}

Mat4 Mat4::translate(f32 x, f32 y, f32 z)
{
    return Mat4
    {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {x, y, z, 1},
        }
    };
}

Mat4 Mat4::scale(Vec3 scale)
{
    return Mat4::scale(scale.x, scale.y, scale.z);
}

Mat4 Mat4::scale(f32 amount)
{
    return scale(amount, amount, amount);
}

Mat4 Mat4::scale(f32 x, f32 y, f32 z)
{
    return Mat4
    {
        {
            {x, 0, 0, 0},
            {0, y, 0, 0},
            {0, 0, z, 0},
            {0, 0, 0, 1},
        }
    };
}

Mat4 Mat4::rotateX(f32 amount)
{
    return Mat4
    {
        {
            {1, 0, 0, 0},
            {0, cos(amount), sin(amount), 0},
            {0, -sin(amount), cos(amount), 0},
            {0, 0, 0, 1},
        }
    };
}

Mat4 Mat4::rotateY(f32 amount)
{
    amount *= -1;
    return Mat4
    {
        {
            {cos(amount), 0, -sin(amount), 0},
            {0, 1, 0, 0},
            {sin(amount), 0, cos(amount), 0},
            {0, 0, 0, 1},
        }
    };
}

Mat4 Mat4::rotateZ(f32 amount)
{
    amount *= -1;
    return Mat4
    {
        {
            {cos(amount), sin(amount), 0, 0},
            {-sin(amount), cos(amount), 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    };
}

Mat4 Mat4::rotate(f32 x, f32 y, f32 z)
{
    return rotateX(x) * rotateY(y) * rotateZ(z);
}

Mat4 Mat4::rotate(Vec3 euler)
{
    return rotate(euler.x, euler.y, euler.z);
}

Mat4 Mat4::trs(Vec3 offset, Vec3 euler, Vec3 scale)
{
    return Mat4::scale(scale) * rotate(euler) * translate(offset);
}

Mat4 Mat4::perspective(f32 near, f32 far, s32 screenWidth, s32 screenHeight, f32 fov)
{
    f32 aspect_ratio = static_cast<f32>(screenWidth) / static_cast<f32>(screenHeight);
    f32 top = std::tan(fov * DEG2RAD / 2) * near;
    f32 bottom = -top;
    f32 right = top * aspect_ratio;
    f32 left = -top * aspect_ratio;

    return Mat4
    {
        {
            {2 * near / (right - left), 0, 0, 0},
            {0, 2 * near / (top - bottom), 0, 0},
            {(right + left)/(right - left), (top + bottom)/(top - bottom), -(far + near)/(far - near), -1},
            {0, 0, -(2 * near * far)/(far - near), 0},
        }
    };
}

Mat4 Mat4::orthographic(f32 near, f32 far, s32 screenWidth, s32 screenHeight, f32 size)
{
    f32 aspect_ratio = static_cast<f32>(screenWidth) / static_cast<f32>(screenHeight);
    f32 right = size * aspect_ratio;
    f32 top = size;
    f32 left = -right;
    f32 bottom = -top;

    return Mat4
    {
        {
            {2 / (right - left), 0, 0, 0},
            {0, 2 / (top - bottom), 0, 0},
            {0, 0, -2 / (far - near), 0},
            {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1},
        }
    };
}

Mat4 Mat4::orthographic(f32 near, f32 far, f32 top, f32 bottom, f32 left, f32 right)
{
    return Mat4
    {
        {
            {2 / (right - left), 0, 0, 0},
            {0, 2 / (top - bottom), 0, 0},
            {0, 0, -2 / (far - near), 0},
            {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1},
        }
    };
}

// === Constants ===
const Mat4 Mat4::IDENTITY
{
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }
};

const Mat4 Mat4::ZERO
{
    {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    }
};
