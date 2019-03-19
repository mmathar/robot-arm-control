#include "RobotMotor.h"
#include "MathUtil.h"

namespace RobotTools
{
    const unsigned short MIN_PULSE_LENGTH = 0;
    const unsigned short MAX_PULSE_LENGTH = 4096;
    const short MIN_DEGREES = -360;
    const short MAX_DEGREES = 360;

    Motor::Motor(Adafruit_PWMServoDriver& driver, ServoNumber pin)
        : driver(driver)
        , pin(pin)
        // these seem to be sensible defaults for the motors I have
        // (MG996R and SG90)
        , minPulse(190)
        , maxPulse(470)
        , minDegrees(0)
        , maxDegrees(180)
        , restDegrees(90)
        , currentAngle(restDegrees)
    {

    }

    void Motor::setMinimum(unsigned short pulseLength, short degrees)
    {
        minPulse = clamp(MIN_PULSE_LENGTH, pulseLength, MAX_PULSE_LENGTH);
        minDegrees = clamp(MIN_DEGREES, degrees, MAX_DEGREES);
    }

    void Motor::setMaximum(unsigned short pulseLength, short degrees)
    {
        maxPulse = clamp(MIN_PULSE_LENGTH, pulseLength, MAX_PULSE_LENGTH);
        maxDegrees = clamp(MIN_DEGREES, degrees, MAX_DEGREES);
    }

    short Motor::getMinimumAngle() const
    {
        return minDegrees;
    }

    short Motor::getMaximumAngle() const
    {
        return maxDegrees;
    }

    float Motor::getMinimumAngleRad() const
    {
        return minDegrees / 180.0f * M_PI;
    }

    float Motor::getMaximumAngleRad() const
    {
        return maxDegrees / 180.0f * M_PI;
    }


    void Motor::setRestPosition(short degrees)
    {
        restDegrees = clamp(MIN_DEGREES, degrees, MAX_DEGREES);
    }

    void Motor::rotateTo(float degrees)
    {
        degrees = clamp((float)minDegrees, degrees, (float)maxDegrees);
        if(minDegrees <= maxDegrees && minPulse <= maxPulse)
        {
            unsigned short pulseLength = degreesToPulse(degrees);
            driver.setPWM(pin, 0, pulseLength);
            currentAngle = degrees;
        }
    }

    void Motor::rotateRelativeTo(float value)
    {
        value = clamp(0.0f, value, 1.0f);
        if(minDegrees <= maxDegrees && minPulse <= maxPulse)
        {
            // linearly interpolate between the minimum and maximum pulse length
            // value = 0.0 -> minPulse
            // value = 1.0 -> maxPulse
            unsigned short pulseLength = minPulse + (unsigned short)((maxPulse - minPulse) * value);
            driver.setPWM(pin, 0, pulseLength);
            float angle = minDegrees + (maxDegrees - minDegrees) * value;
            currentAngle = angle;
        }
    }

    void Motor::returnToRest()
    {
        rotateTo(restDegrees);
    }

    // map an angle (value) from [minDegrees, maxDegrees] to [minPulse, maxPulse]
    unsigned short Motor::degreesToPulse(float value)
    {
        return (unsigned short)((value - minDegrees) * (maxPulse - minPulse) / (maxDegrees - minDegrees) + minPulse);
    }

    float Motor::getAngle()
    {
        return currentAngle;
    }
}; // namespace RobotTools
