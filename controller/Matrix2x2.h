#ifndef MATRIX2X2_H
#define MATRIX2X2_H

namespace RobotTools
{
    class Matrix2x2 
    {
    public:
        // mij is in row i column j
        Matrix2x2(float m00, float m01, float m10, float m11);
        Matrix2x2();

        // careful: only the value 0 and 1 for row and column are allowed!
        // this is nothing more than a glorified array access
        float& at(short row, short column);
        Matrix2x2 inverse();
        float determinant();

    private:
        float values[4];
    }; // class Matrix2x2
}; // namespace RobotTools

#endif // MATRIX2X2_H