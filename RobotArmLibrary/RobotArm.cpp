#include "RobotArm.h"

#include "Matrix2x2.h"
#include "Vector2.h"
#include "MathUtil.h"

namespace RobotTools
{
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
        mainArmMotor.setMinimum(280, -5);
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
        updateCurrentPosition();
    }

    float RobotArm::getArmLength() const
    {
        return 14.7; // cm, for both the main and small arm
    }

    // the distance the gripper will be from the rest position (in z-direction) 
    // if the horizontal arm has inclination alpha and the vertical (main) arm inclination beta.
    // NOTE: 
    //      beta corresponds to the angle mainArmMotor has. 
    //      alpha does NOT correspond to the angle smallArmMotor has. (the angle for the motor is 90° - alpha)
    float RobotArm::calculateDistance(float alpha, float beta) 
    {
        return getArmLength() * (sin(beta) + cos(alpha) - 1);
    }

    // the height the gripper will be at (distance in y-direction from rest position)
    // if the horizontal arm has inclination alpha and the vertical (main) arm inclination beta.
    // NOTE: 
    //      beta corresponds to the angle mainArmMotor has. 
    //      alpha does NOT correspond to the angle smallArmMotor has. (the angle for the motor is 90° - alpha)
    float RobotArm::calculateHeight(float alpha, float beta)
    {
        return getArmLength() * (sin(alpha) - cos(beta) + 1);
    }

    // Move the gripper to the specified position in "robot coordinates".
    GripperPosition RobotArm::moveGripperTo(GripperPosition position)
    {
        // Target positions outside of what the robot arm can reach will produce strange results.
        // To remedy this project these positions back into the acceptable area.
        //projectIntoWorkingArea(position);

        // Starting angles for the approximation chosen to be kind of "centered"
        float alpha = 0.2f * M_PI; // inclination angle of the horizontal arm ("small arm")
        float beta =  0.2f * M_PI; // inclination angle of the vertical arm ("main arm")
        // NOTE: 
        //      beta corresponds to the angle mainArmMotor has. 
        //      alpha does NOT correspond to the angle smallArmMotor has. (the angle for the motor is 90° - alpha)

        const float eps = 1e-6f;
        const float halfEps = 0.5f * eps;

        // Here's the deal:
        // Moving to a specific position (and not just a specific angle) is not trivial
        // because the two arms of the robot both move the gripper in 'distance' and 'height' direction.
        // Therefore, the correct angle for both arms needs to found.
        // The base rotation and gripper status map one to one to the motors, though.

        // -> Do a few iterations of Newton's method to approximate the correct angles
        // https://en.wikipedia.org/wiki/Newton%27s_method#k_variables,_k_functions
        for(int i = 5; i >= 0; i--) 
        {
            // update errors using current alpha and beta appoximations
            float distanceError = position.distance - calculateDistance(alpha, beta);
            float heightError = position.height - calculateHeight(alpha, beta);

            // compute jacobi matrix by finite differences
            Matrix2x2 jacobi;
            jacobi.at(0, 0) = (calculateDistance(alpha + halfEps, beta) - calculateDistance(alpha - halfEps, beta)) / eps;
            jacobi.at(0, 1) = (calculateDistance(alpha, beta + halfEps) - calculateDistance(alpha, beta - halfEps)) / eps;
            jacobi.at(1, 0) = (calculateHeight(alpha + halfEps, beta) - calculateHeight(alpha - halfEps, beta)) / eps;
            jacobi.at(1, 1) = (calculateHeight(alpha, beta + halfEps) - calculateHeight(alpha, beta - halfEps)) / eps;

            if(abs(jacobi.determinant())  < 1e-3)
                break; // solution won't get better at this point

            // now solve the equation jacobi * (delta_alpha delta_beta)^T = -(distanceError heightError)^T
            // by computing (delta_alpha delta_beta)^T = (jacobi^-1) * -(distanceError heightError)^T
            // and compute (alpha beta)^T = (alpha beta)^T + (delta_alpha delta_beta)^T.
            // Damping was added because otherwise the solution oscillates out of control.
            Matrix2x2 inverseJacobi = jacobi.inverse();
            const float dampingFactor = 0.8f;
            alpha += dampingFactor * (inverseJacobi.at(0, 0) * distanceError + inverseJacobi.at(0, 1) * heightError);
            beta += dampingFactor * (inverseJacobi.at(1, 0) * distanceError + inverseJacobi.at(1, 1) * heightError);
        }

        // check for nasties
        if(isnan(alpha) || isinf(alpha) || isnan(beta) || isinf(beta))
            return currentPosition;
        
        // 90.0 - alpha because the small arm motor is mounted from the other side (compared to the mainArmMotor)
        // -> it turns in the opposite direction
        smallArmMotor.rotateTo(90.0f - radToDegree(alpha));   
        mainArmMotor.rotateTo(radToDegree(beta));
        baseMotor.rotateTo(position.rotation);

        updateCurrentPosition();
        return currentPosition;
    }

    // update the position from the actual motor positions
    // this is necessary because the robot movement does not necessarily correspond to what was 
    // given to the last call of moveGripperTo (small errors occurr and the point may have been outside of the working area)
    void RobotArm::updateCurrentPosition()
    {
        float alpha = (90.0f - smallArmMotor.getAngle()) / 180.0f * M_PI;
        float beta = mainArmMotor.getAngle() / 180.0f * M_PI;
        currentPosition.rotation = baseMotor.getAngle();
        currentPosition.distance = calculateDistance(alpha, beta);
        currentPosition.height = calculateHeight(alpha, beta);
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

    bool RobotArm::isGripperOpen()
    {
        return gripperMotor.getAngle() >= 45.0f; // more than halfway open?
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
        updateCurrentPosition();
    }

    void RobotArm::setDirectMainArmRotation(const double value)
    {
        mainArmMotor.rotateTo(value);
        updateCurrentPosition();
    }

    void RobotArm::setDirectBaseRotation(const double value)
    {
        baseMotor.rotateTo(value);
        updateCurrentPosition();
    }

    void RobotArm::setDirectGripperRotation(const double value)
    {
        gripperMotor.rotateTo(value);
        updateCurrentPosition();
    }

    void GripperPosition::rotateBase(float angle)
    {
        rotation += angle;
    }

    void GripperPosition::moveForward(float distance)
    {
        this->distance += distance;
    }

    void GripperPosition::moveBackward(float distance)
    {
        this->distance -= distance;
    }

    void GripperPosition::moveUp(float distance)
    {
        this->height -= distance;
    }

    void GripperPosition::moveDown(float distance)
    {
        this->height += distance;
    }

}; // namespace RobotTools
