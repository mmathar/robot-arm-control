#include "RobotArm.h"

using namespace EEZYBot;

/* 
BaseMotor: pin 0
MainArmMotor: pin 2
SmallArmMotor: pin 1
GripperMotor: pin 3
*/
RobotArm::RobotArm()
    : driver()
    , baseMotor(driver, SERVO_0)
    , mainArmMotor(driver, SERVO_2)
    , smallArmMotor(driver, SERVO_1)
    , gripperMotor(driver, SERVO_3)
    , currentPosition{0.0, 0.0, 0.0}
    //, workAreaEdges({})
{
}

/*void RobotArm::initializeWorkArea()
{
    for(int i = 0; i < 10; i++) 
    {
        float beta = mainArmMotor.getMinimumAngle() + (mainArmMotor.getMaximumAngle() - mainArmMotor.getMinimumAngle()) / 9.0f * i;
        Vector2 position = calcPosition(0.0f, beta);
        Vector2 positionNext = calcPosition(0.0f, beta + 5.0f);
        Edge edge = 
    }
}*/

/*!  The specs in this method need to be tuned according to your specific motor.
* e.g. 
*   - get the PWM frequency from the specs
*   - calibrating the minimum/maximum pulse length manually 
*     (test how far each motor can move and if you hit the hard-limit tune it back a bit)
*/
void RobotArm::setup()
{
    driver.begin();  
    driver.setPWMFreq(50); // according to MG996R specs
    delay(10);  

    // Manually calibrated values:
    // Base rotates -45..45 degrees left/right from center
    baseMotor.setMinimum(190, -45);
    baseMotor.setMaximum(470, 45);
    baseMotor.setRestPosition(0);  
    // The main arm starts upright (0°) and can rotate 90°
    smallArmMotor.setMinimum(240, 45);
    smallArmMotor.setMaximum(330, 90);
    smallArmMotor.setRestPosition(90);   
    // The small helper arm starts horizontally (0°) and can rotate 90°
    mainArmMotor.setMinimum(280, -10);
    mainArmMotor.setMaximum(420, 50);
    mainArmMotor.setRestPosition(0);   
    // The gripper has an opening (half-)angle of 90° (open) and a value of 0° closed
    gripperMotor.setMinimum(190, 0);
    gripperMotor.setMaximum(370, 90);
    gripperMotor.setRestPosition(90); // in the rest position the gripper is open
}

void RobotArm::returnToRest()
{
    baseMotor.returnToRest();
    mainArmMotor.returnToRest();
    smallArmMotor.returnToRest();
    gripperMotor.returnToRest();
}

float clamp(float low, float x, float high)
{
    if(x < low)
        return low;
    if(x > high)
        return high;
   // if(isnan(x))
   //     return low;
   // if(isinf(x))
   //     return high;
    return x;
}

struct Matrix2x2 
{
    // mij is row i column j
    Matrix2x2(float m00, float m01, float m10, float m11)
        : values{m00, m01, m10, m11}
    {

    }

    Matrix2x2() : Matrix2x2(1.0f, 0.0f,
                            0.0f, 1.0f) 
    {

    }

    float values[4];

    float& at(short row, short column) 
    {
        #ifdef _DEBUG
        if(row < 0 || row > 1) 
        {
            Serial.print("Matrix4x4::at row index out of range.");
            return 0.0f;
        } 
        else if(column < 0 || column > 1)
        {
            Serial.print("Matrix4x4::at column index out of range.");
            return 0.0f;
        }
        #endif
        
        return values[row * 2 + column];
    }

    Matrix2x2 inverse() 
    {
        Matrix2x2 result;
        float factor = 1.0f / (determinant() + 1e-6) ;
        result.at(0, 0) = factor *  at(1, 1);
        result.at(0, 1) = factor * -at(0, 1);
        result.at(1, 0) = factor * -at(1, 0);
        result.at(1, 1) = factor *  at(0, 0);
        return result;
    }

    float determinant()
    {
        return at(0, 0) * at(1, 0) - at(0, 1) * at(1, 0);
    }
};

float getArmLength()
{
    return 14.7; // cm, for both the main and small arm
}

float calcDistance(float alpha, float beta) 
{
    return getArmLength() * (sin(beta) + cos(alpha) - 1);
}

float calcHeight(float alpha, float beta)
{
    return getArmLength() * (sin(alpha) - cos(beta) + 1);
}

void RobotArm::moveGripperTo(GripperPosition position)
{
    float beta = 0.25f * 3.1415f;
    float alpha = 0.2f * 3.1415f;

    projectIntoWorkingArea(position);

    float eps = 1e-6f;
    float halfEps = 0.5f * eps;

    for(int i = 5; i >= 0; i--) 
    {
        float currentDistance = calcDistance(alpha, beta);
        float currentHeight = calcHeight(alpha, beta);
        float distError = (position.distance - currentDistance);
        float heightError = (position.height - currentHeight);

        // compute jacobi matrix by finite differences
        Matrix2x2 jacobi;
        jacobi.at(0, 0) = (calcDistance(alpha +  halfEps, beta) - calcDistance(alpha - halfEps, beta)) / eps;
        jacobi.at(0, 1) = (calcDistance(alpha, beta + halfEps) - calcDistance(alpha, beta - halfEps)) / eps;
        jacobi.at(1, 0) = (calcHeight(alpha + halfEps, beta) - calcHeight(alpha - halfEps, beta)) / eps;
        jacobi.at(1, 1) = (calcHeight(alpha, beta + halfEps) - calcHeight(alpha, beta - halfEps)) / eps;

        if(abs(jacobi.determinant())  < 1e-3)
            break; // solution won't get better at this point

        // now solve the equation jacobi * (delta_alpha delta_beta)^T = -(distError heightError)^T
        // by computing (delta_alpha delta_beta)^T = (jacobi^-1) * -(distError heightError)^T
        // and compute (alpha beta)^T = (alpha beta)^T + (delta_alpha delta_beta)^T.
        // Damping was added because otherwise the solution oscillates out of control.
        Matrix2x2 inverseJacobi = jacobi.inverse();
        const float dampingFactor = 0.8f;
        alpha += dampingFactor * (inverseJacobi.at(0, 0) * distError + inverseJacobi.at(0, 1) * heightError);
        beta += dampingFactor * (inverseJacobi.at(1, 0) * distError + inverseJacobi.at(1, 1) * heightError);

        alpha = clamp(0.0f, alpha, 0.25f * 3.1415f);
        beta = clamp(-10.0f / 180.0f * 3.1415f, beta, 50.0f / 180.0f * 3.1415f);
    }

    alpha = clamp(0.0f, alpha, 0.25f * 3.1415f);
    beta = clamp(-10.0f / 180.0f * 3.1415f, beta, 50.0f / 180.0f * 3.1415f);

    float errorDist = abs(position.distance - getArmLength() * (sin(beta) + cos(alpha) - 1));
    float errorHeight = abs(position.height - getArmLength() * (sin(alpha) - cos(beta) + 1));
    float totalError = errorDist * errorDist + errorHeight * errorHeight;

    if(isnan(alpha) || isinf(alpha) || isnan(beta) || isinf(beta))
        return;

    mainArmMotor.rotateTo(beta / 3.1415f * 180.0f);
    // 90.0 - alpha because the small arm motor is mounted from the other side (compared to the mainArmMotor)
    // -> it turns in the opposite direction
    smallArmMotor.rotateTo(90.0f - (alpha / 3.1415f * 180.0f));   
    gripperMotor.rotateTo(position.gripper); 
    baseMotor.rotateTo(position.rotation);

    currentPosition = position;
}

void RobotArm::projectIntoWorkingArea(GripperPosition& position) 
{
    float distanceDelta = position.distance - currentPosition.distance;
    float heightDelta = position.height - currentPosition.height;

    // upper right "quadrant"
    if(position.distance >= position.height) 
    {

    }
    else 
    {
        // ... 3 other cases
    }
}

void RobotArm::openGripper()
{
    gripperMotor.rotateTo(90);
}

void RobotArm::closeGripper()
{
    gripperMotor.rotateTo(0);
}

void RobotArm::setGripperOpeningAngle(float angle)
{
    gripperMotor.rotateTo(angle);
}

GripperPosition RobotArm::getCurrentPosition()
{
    return currentPosition;
}


void RobotArm::setDirectSmallArmRotation(const double value)
{
    smallArmMotor.rotateTo(value);
}

void RobotArm::setDirectMainArmRotation(const double value)
{
    mainArmMotor.rotateTo(value);
}

void RobotArm::setDirectBaseRotation(const double value)
{
    baseMotor.rotateTo(value);
}

void RobotArm::setDirectGripperRotation(const double value)
{
    gripperMotor.rotateTo(value);
}
