#include "../string_util.h"
#include "math_util.h"
#include "vec3.h"

// === Lifetime ===
vec3::vec3(f32 x, f32 y, f32 z) : x {x}, y {y}, z{z}
{
}

// === Operations ===
vec3 vec3::cross(vec3 first, vec3 second)
{
    float x = first.y * second.z - first.z * second.y;
    float y = first.z * second.x - first.x * second.z;
    float z = first.x * second.y - first.y * second.x;
    return vec3{x, y, z};
}

// === Constants ===
const vec3 vec3::ZERO(0, 0, 0);
const vec3 vec3::ONE(1, 1, 1);
const vec3 vec3::UP(0, 1, 0);
const vec3 vec3::DOWN(0, -1, 0);
const vec3 vec3::LEFT(-1, 0, 0);
const vec3 vec3::RIGHT(1, 0, 0);
const vec3 vec3::FORWARD(0, 0, 1);
const vec3 vec3::BACK(0, 0, -1);

// === Operators ===
bool vec3::operator==(vec3 other) const
{
    return math::nearlyEqual(x, other.x)
        && math::nearlyEqual(y, other.y)
        && math::nearlyEqual(z, other.z);
}

std::ostream& vec3::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f, %.1f}", x, y, z);
}
