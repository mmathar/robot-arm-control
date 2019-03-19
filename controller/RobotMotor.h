#ifndef ROBOTMOTOR_H
#define ROBOTMOTOR_H

#include <Adafruit_PWMServoDriver.h>

namespace RobotTools
{
    // Used to restrict the values for "pin" in the constructor to allowed ones
    // (the Servo Driver has 16 pins) 
    enum ServoNumber
    {
        SERVO_0 = 0,
        SERVO_1,
        SERVO_2,
        SERVO_3,
        SERVO_4,
        SERVO_5,
        SERVO_6,
        SERVO_7,
        SERVO_8,
        SERVO_9,
        SERVO_10,
        SERVO_11,
        SERVO_12,
        SERVO_13,
        SERVO_14,
        SERVO_15
    };

    /*! Abstraction of PWM stuff (from the Adafruit_PWMServoDriver)
    * There is an inbuilt Servo.h Arduino library but it does not work (?)
    * for the Adafruit PCA9685 16-Channel Servo Driver (I think)
    */
    class Motor
    {
    public:
        // Which pins on the Servo Driver are used? (0-11)
        Motor(Adafruit_PWMServoDriver& driver, ServoNumber pin);

        // CONFIGURATION
        //  Both of these NEED to be called before the servo can be used
        //  Set the minimum and maximum pulse length values (specific to servo type, you might want to calibrate these)
        //  and the corresponding rotational position in degrees.
        // CAREFUL: 
        //  The minimum for both pulseLength and degrees needs to be smaller than the respective maximum. 
        //  Otherwise the motor won't do anything.
        // Arguments:
        //  pulseLength is limited to [0, 4096]
        //  degrees is limited to [-360, 360]
        // Example:
        //  Motor motor(SERVO_1);
        //  motor.setMinimum(190, 0);
        //  motor.setMaximum(470, 180);
        void setMinimum(unsigned short pulseLength, short degrees);
        void setMaximum(unsigned short pulseLength, short degrees);
        short getMinimumAngle() const;
        short getMaximumAngle() const;
        float getMinimumAngleRad() const;
        float getMaximumAngleRad() const;

        // optional. Set the position the servo will start at and return to if returnToRest is called.
        void setRestPosition(short degrees);

        // CONTROL
        // Rotate to position described by "degree" as configured using
        // setMinimum / setMaximum
        void rotateTo(float degree);

        // rotate to position described by a value between 0 (= minimum) and 1 (= maximum)
        void rotateRelativeTo(float value);

        // move motor to default position
        void returnToRest();
        // get the current rotational angle
        float getAngle();

    private:

        // compute the pulse length from a given angle
        unsigned short degreesToPulse(float value);

    private:
        Adafruit_PWMServoDriver& driver;
        ServoNumber pin;
        unsigned short minPulse;
        unsigned short maxPulse;
        short minDegrees;
        short maxDegrees;
        short restDegrees;
        float currentAngle;
    }; // class RobotMotor
}; // namespace RobotTools

#endif // ROBOTMOTOR_H