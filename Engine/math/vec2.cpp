#include "vec2.h"
#include "../string_util.h"

// === Lifetime ===
vec2::vec2(f32 x, f32 y) : x{x}, y{y}
{
}

// === Operators ===
bool vec2::operator==(vec2 other) const
{
    return math::nearlyEqual(x, other.x)
        && math::nearlyEqual(y, other.y);
}

std::ostream& vec2::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f}", x, y);
}

// === Constants ===
const vec2 vec2::ZERO(0, 0);
const vec2 vec2::ONE(1, 1);
const vec2 vec2::UP(0, 1);
const vec2 vec2::DOWN(0, -1);
const vec2 vec2::LEFT(-1, 0);
const vec2 vec2::RIGHT(1, 0);
