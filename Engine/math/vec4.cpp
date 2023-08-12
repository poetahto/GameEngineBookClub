#include "vec4.h"
#include "../string_util.h"

Vec4::Vec4(f32 x, f32 y, f32 z, f32 w) : x{x}, y{y}, z{z}, w{w}
{
}

bool Vec4::operator==(Vec4 other) const
{
    return Math::nearlyEqual(other.x, x)
        && Math::nearlyEqual(other.y, y)
        && Math::nearlyEqual(other.z, z)
        && Math::nearlyEqual(other.w, w);
}

std::ostream& Vec4::operator<<(std::ostream& os) const
{
    return os << stringFormat("{%.1f, %.1f, %.1f, %.1f}", x, y, z, w);
}
