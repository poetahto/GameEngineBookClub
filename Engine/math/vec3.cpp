#include <complex>
#include "math_util.h"
#include "vec3.h"

float vec3::magnitude() const
{
    return std::sqrt(magnitudeSqr());
}

float vec3::magnitudeSqr() const
{
    return x * x + y * y + z * z;
}

void vec3::normalize()
{
    float m = magnitude();

    if (!math::nearlyEqual(m, 0))
    {
        x /= m;
        y /= m;
        z /= m;
    }
}

vec3 vec3::normalized() const
{
    vec3 result = vec3(*this);
    result.normalize();
    return result;
}

float dot(vec3 first, vec3 second)
{
    return
        first.x * second.x +
        first.y * second.y +
        first.z * second.z;
}

vec3 cross(vec3 first, vec3 second)
{
    float x = first.y * second.z - first.z * second.y;
    float y = first.z * second.x - first.x * second.z;
    float z = first.x * second.y - first.y * second.x;
    return vec3 { x, y, z };
}

vec3 lerp(vec3 a, vec3 b, float t)
{
    float x = (1 - t) * a.x + t * b.x;
    float y = (1 - t) * a.y + t * b.y;
    float z = (1 - t) * a.z + t * b.z;
    return vec3 { x, y, z };
}

// Equality
bool operator ==(vec3 first, vec3 second)
{
    return math::nearlyEqual(first.x, first.x)
        && math::nearlyEqual(first.y, second.y)
        && math::nearlyEqual(first.z, second.z);
}

// Inequality
bool operator !=(vec3 first, vec3 second)
{
    return !(first == second);
}

// Scalar Multiplication
vec3 operator *(vec3 vector, float k)
{
    return vec3
    {
        vector.x * k,
        vector.y * k,
        vector.z * k,
    };
}

vec3 operator *(float k, vec3 vector)
{
    return vector * k;
}

vec3 operator*=(vec3& first, float k)
{
    first = first * k;
    return first;
}

// Addition
vec3 operator +(vec3 first, vec3 second)
{
    return vec3
    {
        first.x + second.x,
        first.y + second.y,
        first.z + second.z,
    };
}

vec3 operator+=(vec3& a, vec3 b)
{
    a = a + b;
    return a;
}

// Subtraction
vec3 operator -(vec3 first, vec3 second)
{
    return vec3
    {
        first.x - second.x,
        first.y - second.y,
        first.z - second.z,
    };
}

vec3 operator-=(vec3& a, vec3 b)
{
    a = a - b;
    return a;
}

// Negation
vec3 operator -(vec3 vector)
{
    return vec3{-vector.x, -vector.y, -vector.z};
}
