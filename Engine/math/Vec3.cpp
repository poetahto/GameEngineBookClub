#include "../string_util.hpp"
#include "math_util.hpp"
#include "vec3.hpp"

// === Lifetime ===
Vec3::Vec3(f32 x, f32 y, f32 z) : x {x}, y {y}, z{z}
{
}

// === Operations ===
Vec3 Vec3::cross(Vec3 first, Vec3 second)
{
    float x = first.y * second.z - first.z * second.y;
    float y = first.z * second.x - first.x * second.z;
    float z = first.x * second.y - first.y * second.x;
    return Vec3{x, y, z};
}

// === Constants ===
const Vec3 Vec3::ZERO(0, 0, 0);
const Vec3 Vec3::ONE(1, 1, 1);
const Vec3 Vec3::UP(0, 1, 0);
const Vec3 Vec3::DOWN(0, -1, 0);
const Vec3 Vec3::LEFT(-1, 0, 0);
const Vec3 Vec3::RIGHT(1, 0, 0);
const Vec3 Vec3::FORWARD(0, 0, 1);
const Vec3 Vec3::BACK(0, 0, -1);

// === Operators ===
bool Vec3::operator==(Vec3 other) const
{
    return Math::nearlyEqual(x, other.x)
        && Math::nearlyEqual(y, other.y)
        && Math::nearlyEqual(z, other.z);
}

std::ostream& Vec3::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f, %.1f}", x, y, z);
}
