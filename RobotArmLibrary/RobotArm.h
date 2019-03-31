#ifndef ROBOTARM_H
#define ROBOTARM_H

#include <Adafruit_PWMServoDriver.h>

#include "RobotMotor.h"
#include "Vector2.h"

namespace RobotTools
{
    /*! Describe the position (both actual and wanted) as polar coordinates:
    */
    struct GripperPosition
    {
        float rotation; // roation around the robots main axis (equivalent to rotation of baseMotor)
        float height;  // the height the gripper is at. 0 at rest position
        float distance; // how far extended the gripper is. 0 at rest position 

        // helper functions to modify position for next frame
        // movement limited to one axis (logical, not physical axis)
        void rotateBase(float angle);
        void moveForward(float distance);
        void moveBackward(float distance);
        void moveUp(float distance);
        void moveDown(float distance);  
    };

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
       // returns the position the gripper actually ended up at (might be different from the passed position)
       GripperPosition moveGripperTo(GripperPosition position);

       // fully open the gripper
       void openGripper();
       // fully close the gripper
       void closeGripper();
       bool isGripperOpen();
       // open/close the gripper to the specified half-angle
       void setGripperOpeningAngle(float halfAngle); 
       GripperPosition getCurrentPosition();

       // returns the length of both the primary and secondary arm
       // (they are the same length)
       float getArmLength() const;

       // set the angles for the motors directly. Mostly useful for debugging.
       // Will screw up things if mixed with "moveGripperTo" calls.
       void setDirectSmallArmRotation(const double value);
       void setDirectMainArmRotation(const double value);
       void setDirectBaseRotation(const double value);
       void setDirectGripperRotation(const double value);
    
      private:
        void projectIntoWorkingArea(GripperPosition& position);
        void initializeWorkArea();
        float calculateDistance(float alpha, float beta);
        float calculateHeight(float alpha, float beta);
        void updateCurrentPosition();

      private:
        // the servos making up the robot arm
        Adafruit_PWMServoDriver driver;
        Motor baseMotor;
        Motor mainArmMotor;
        Motor smallArmMotor;
        Motor gripperMotor;
        GripperPosition currentPosition;
    }; // class RobotArm
}; // namespace RobotTools

#endif
