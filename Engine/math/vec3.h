#ifndef VECTOR_H
#define VECTOR_H

#include "../types.h"

struct vec3
{
    f32 x;
    f32 y;
    f32 z;

    vec3(f32 x, f32 y, f32 z) : x{x}, y{y}, z{z}
    {
    }

    float magnitude() const;
    float magnitudeSqr() const;
    void normalize();
    vec3 normalized() const;

    static float dot(vec3 first, vec3 second);
    static vec3 cross(vec3 first, vec3 second);
    static vec3 lerp(vec3 a, vec3 b, float t);

    // === Constants ===
    static const vec3 ZERO;
    static const vec3 ONE;
    static const vec3 UP;
    static const vec3 DOWN;
    static const vec3 LEFT;
    static const vec3 RIGHT;
    static const vec3 FORWARD;
    static const vec3 BACK;
};

// === Operators ===
bool operator ==(vec3 first, vec3 second); // equality
bool operator !=(vec3 first, vec3 second); // inequality
vec3 operator *(vec3 vector, float k); // scalar multiplication
vec3 operator *(float k, vec3 vector);
vec3 operator *=(vec3& first, float k);
vec3 operator +(vec3 first, vec3 second); // addition
vec3 operator +=(vec3& a, vec3 b);
vec3 operator -(vec3 first, vec3 second); // subtraction
vec3 operator -=(vec3& a, vec3 b);
vec3 operator -(vec3 vector); // negation
std::ostream& operator<<(std::ostream& os, const vec3& value);

#endif // VECTOR_H
