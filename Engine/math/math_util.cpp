#include <algorithm>
#include "math_util.h"

using namespace math;

// Taken from: https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
bool math::nearlyEqual(float a, float b)
// those defaults are arbitrary and could be removed
{
    float maxXYOne = std::max( { 1.0f, std::abs(a) , std::abs(b) } ) ;
    return std::abs(a - b) <= std::numeric_limits<float>::epsilon()*maxXYOne ;
}
