#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <math.h>

namespace RobotTools
{
    // restrict value to the interval [low, high]
    template <typename T>
    T clamp(const T& low, const T& value, const T& high) {
        return max(low, min(value, high));
    }

    // convert radiant to degrees
    float radToDegree(float value) 
    {
        return value / M_PI * 180.0f;
    }
}; // namespace RobotTools

#endif // MATHUTIL_H