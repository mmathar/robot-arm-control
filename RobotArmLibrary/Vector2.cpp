#include "Vector2.h"

#include <math.h>

namespace RobotTools
{
    Vector2::Vector2(float x, float y) 
        : x(x)
        , y(y) 
    {

    }

    float Vector2::length()
    {
        return sqrt(x * x + y * y);
    }

    void Vector2::normalize() 
    {
        float factor = 1.0f / length();
        x *= factor;
        y *= factor;
    }
}; // namespace RobotTools
