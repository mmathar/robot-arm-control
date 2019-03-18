#ifndef ROBOTARM_H
#define ROBOTARM_H

#include <Adafruit_PWMServoDriver.h>

#include "RobotMotor.h"

namespace EEZYBot
{
    /*! Describe the position (both actual and wanted) as polar coordinates:
    */
    struct GripperPosition
    {
        float rotation; // roation around the robots main axis (equivalent to rotation of baseMotor)
        float height;  // the height the gripper is at (0 = lowest possible, 1 = highest possible)
        float distance; // how far extended the gripper is (0 = closest possible to robot, 1 = furthest possible) 
        float gripper;      
    };

    /*struct Vector2
    {
        Vector2(float x, float y) : x(x), y(y) {}

        float x, y;

        float length()
        {
            return sqrt(x * x + y * y);
        }

        void normalize() 
        {
            float factor = 1.0f / length();
            x *= factor;
            y *= factor;
        }
    };

    struct Edge
    {
        Vector2 start, end;
    };*/

    /*! Control class for an EEZYbotARM MK2
        (http://www.eezyrobots.it/eba_mk2.html)

        Translates "move to position (X, Y, Z)" commands into appropriate servo positions.
    */
    class RobotArm
    {
    public:
       RobotArm();  
       // needs to be called before use
       void setup(); 
       // returns the robot into the default position
       void returnToRest();
       // move the robot so that the gripper ends up at the specified position
       void moveGripperTo(GripperPosition position);
       // fully open the gripper
       void openGripper();
       // fully close the gripper
       void closeGripper();
       // open/close the gripper to the specified half-angle
       void setGripperOpeningAngle(float halfAngle); 
       GripperPosition getCurrentPosition();

       void setDirectSmallArmRotation(const double value);
       void setDirectMainArmRotation(const double value);
       void setDirectBaseRotation(const double value);
       void setDirectGripperRotation(const double value);
    
      private:
        void projectIntoWorkingArea(GripperPosition& position);
        void initializeWorkArea();

      private:
        // the servos making up the robot arm
        Adafruit_PWMServoDriver driver;
        Motor baseMotor;
        Motor mainArmMotor;
        Motor smallArmMotor;
        Motor gripperMotor;
        GripperPosition currentPosition;
        //Edge workAreaEdges[40];
    };
}; // namespace EEZYBot

#endif
