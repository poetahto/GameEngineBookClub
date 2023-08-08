#include "vec4.h"
#include "../string_util.h"

Vec4::Vec4(f32 x, f32 y, f32 z, f32 w) : x{x}, y{y}, z{z}, w{w}
{
}

bool Vec4::operator==(Vec4 other) const
{
    return math::nearlyEqual(other.x, x)
        && math::nearlyEqual(other.y, y)
        && math::nearlyEqual(other.z, z)
        && math::nearlyEqual(other.w, w);
}

std::ostream& Vec4::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f, %.1f, %.1f}", x, y, z, w);
}
