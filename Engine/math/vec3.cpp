#include "vec3.h"

#include <complex>

#include "math_util.h"

float vec3::magnitude() const
{
    return std::sqrt(magnitudeSqr());
}

float vec3::magnitudeSqr() const
{
    return x * x + y * y + z * z;
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

// Negation
vec3 operator -(vec3 vector)
{
    return vec3 { -vector.x, -vector.y, -vector.z };
}
