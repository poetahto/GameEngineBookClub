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
};

// === Operators ===
bool operator ==(vec3 first, vec3 second); // equality
bool operator !=(vec3 first, vec3 second); // inequality
vec3 operator *(vec3 vector, float k); // scalar multiplication
vec3 operator *(float k, vec3 vector);
vec3 operator +(vec3 first, vec3 second); // addition
vec3 operator -(vec3 first, vec3 second); // subtraction
vec3 operator -(vec3 vector); // negation

// === Constants ===
const vec3 ZERO{0, 0, 0};
const vec3 ONE{1, 1, 1};
const vec3 UP{0, 1, 0};
const vec3 DOWN{0, -1, 0};
const vec3 LEFT{-1, 0, 0};
const vec3 RIGHT{1, 0, 0};
const vec3 FORWARD{0, 0, 1};
const vec3 BACK{0, 0, -1};

#endif // VECTOR_H
