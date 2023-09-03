#include <algorithm>
#include "math_util.h"

using namespace Math;

// Taken from: https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
bool Math::nearlyEqual(float a, float b)
// those defaults are arbitrary and could be removed
{
    // i honestly gave up
    return std::abs(a - b) <= 0.00001f;
}
