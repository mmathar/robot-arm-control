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

    Vector2 operator*(float value, const Vector2& v)
    {
        return {v.x * value, v.y * value};
    }

    Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return {a.x + b.x, a.y + b.y};
    }
}; // namespace RobotTools
