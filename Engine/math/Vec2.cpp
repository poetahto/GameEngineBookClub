#include "vec2.hpp"
#include "../string_util.hpp"

// === Lifetime ===
Vec2::Vec2(f32 x, f32 y) : x{x}, y{y}
{
}

// === Operators ===
bool Vec2::operator==(Vec2 other) const
{
    return Math::nearlyEqual(x, other.x)
        && Math::nearlyEqual(y, other.y);
}

std::ostream& Vec2::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f}", x, y);
}

// === Constants ===
const Vec2 Vec2::ZERO(0, 0);
const Vec2 Vec2::ONE(1, 1);
const Vec2 Vec2::UP(0, 1);
const Vec2 Vec2::DOWN(0, -1);
const Vec2 Vec2::LEFT(-1, 0);
const Vec2 Vec2::RIGHT(1, 0);
