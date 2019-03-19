#include "Matrix2x2.h"

namespace RobotTools
{
    Matrix2x2::Matrix2x2(float m00, float m01, float m10, float m11)
        : values{m00, m01, m10, m11}
    {

    }

    Matrix2x2::Matrix2x2() : Matrix2x2(1.0f, 0.0f, 0.0f, 1.0f) 
    {
    }
    
    Matrix2x2 Matrix2x2::inverse() 
    {
        Matrix2x2 result;
        float factor = 1.0f / (determinant() + 1e-6) ;
        result.at(0, 0) = factor *  at(1, 1);
        result.at(0, 1) = factor * -at(0, 1);
        result.at(1, 0) = factor * -at(1, 0);
        result.at(1, 1) = factor *  at(0, 0);
        return result;
    }
 
    float Matrix2x2::determinant()
    {
        return at(0, 0) * at(1, 0) - at(0, 1) * at(1, 0);
    }

}; // namespace RobotTools