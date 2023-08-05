#include "../string_util.h"
#include "math_util.h"
#include "vec3.h"

std::ostream& operator<<(std::ostream &os, const vec3 &value)
{
    return os << stringFormat("{%.1f, %.1f, %.1f}", value.x, value.y, value.z);
}

vec3::vec3(f32 x, f32 y, f32 z) : x {x}, y {y}, z{z}
{
}

vec3 vec3::cross(vec3 first, vec3 second)
{
    float x = first.y * second.z - first.z * second.y;
    float y = first.z * second.x - first.x * second.z;
    float z = first.x * second.y - first.y * second.x;
    return vec3{x, y, z};
}

bool vec3::operator==(vec3 other) const
{
    return math::nearlyEqual(x, other.x)
        && math::nearlyEqual(y, other.y)
        && math::nearlyEqual(z, other.z);
}

// // Equality
// bool operator ==(vec3 first, vec3 second)
// {
//     return math::nearlyEqual(first.x, first.x)
//         && math::nearlyEqual(first.y, second.y)
//         && math::nearlyEqual(first.z, second.z);
// }

// // Inequality
// bool operator !=(vec3 first, vec3 second)
// {
//     return !(first == second);
// }
//
// // Scalar Multiplication
// vec3 operator *(vec3 vector, float k)
// {
//     return vec3
//     {
//         vector.x * k,
//         vector.y * k,
//         vector.z * k,
//     };
// }
//
// vec3 operator *(float k, vec3 vector)
// {
//     return vector * k;
// }
//
// vec3 operator*=(vec3& first, float k)
// {
//     first = first * k;
//     return first;
// }
//
// // Addition
// vec3 operator +(vec3 first, vec3 second)
// {
//     return vec3
//     {
//         first.x + second.x,
//         first.y + second.y,
//         first.z + second.z,
//     };
// }
//
// vec3 operator+=(vec3& a, vec3 b)
// {
//     return a = a + b;
// }
//
// // Subtraction
// vec3 operator -(vec3 first, vec3 second)
// {
//     return vec3
//     {
//         first.x - second.x,
//         first.y - second.y,
//         first.z - second.z,
//     };
// }
//
// vec3 operator-=(vec3& a, vec3 b)
// {
//     a = a - b;
//     return a;
// }
//
// // Negation
// vec3 operator -(vec3 vector)
// {
//     return vec3{-vector.x, -vector.y, -vector.z};
// }

const vec3 vec3::ZERO(0, 0, 0);
const vec3 vec3::ONE(1, 1, 1);
const vec3 vec3::UP(0, 1, 0);
const vec3 vec3::DOWN(0, -1, 0);
const vec3 vec3::LEFT(-1, 0, 0);
const vec3 vec3::RIGHT(1, 0, 0);
const vec3 vec3::FORWARD(0, 0, 1);
const vec3 vec3::BACK(0, 0, -1);
