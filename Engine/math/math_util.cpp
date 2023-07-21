#include <algorithm>
#include <cfloat>
#include "math_util.h"

using namespace math;

// Taken from: https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
bool math::nearlyEqual(float a, float b)
// those defaults are arbitrary and could be removed
{
    static constexpr float epsilon = 128 * FLT_EPSILON;
    static constexpr float abs_th = FLT_MIN;

    if (a == b) return true;  // NOLINT(clang-diagnostic-float-equal)

    auto diff = std::abs(a - b);
    auto norm = std::min(std::abs(a) + std::abs(b), std::numeric_limits<float>::max());
    // or even faster: std::min(std::abs(a + b), std::numeric_limits<float>::max());
    // keeping this commented out until I update figures below
    return diff < std::max(abs_th, epsilon * norm);
}
