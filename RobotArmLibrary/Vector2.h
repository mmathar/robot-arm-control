#ifndef VECTOR2_H
#define VECTOR2_H

namespace RobotTools
{
    // struct because hiding the raw values is pretty pointless
    // - they are heavily used from the outside, anyway.
    struct Vector2
    {
        Vector2(float x, float y);

        float length();
        void normalize();

        float x, y;
    }; // struct Vector2
}; // namespace RobotTools

#endif // VECTOR2_H