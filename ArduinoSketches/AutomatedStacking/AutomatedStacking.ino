#include <RobotArm.h>

RobotTools::RobotArm arm;

/* Sketch description:
 *  
 * The robot takes a stack of 5 2.5cm cubes and stacks it from the right to the left.
 * At startup the robot also moves it's gripper towards the position of the top  
 * of the right cube-stack for 10 seconds so someone can put the stack of cubes
 * in the right spot.
 */

// move to grab cube from stack (index 0 is the bottom of the stack
void moveToCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.distance = 6.37;
  // the height-axis is upside down! 0 is the height of the robots resting position!
  // 15.0 is the "ground plane" the cubes are resting on
  // 2.5 is the height of a single cube
  position.height = 15.0 - 2.5f * index; // 15.0 is the
  position.height -= 3.0; // initially aim for a bit higher so we can softly put it down
  
  arm.moveGripperTo(position);
  delay(500);
}

void openGripper()
{
  arm.openGripper();
  delay(500);
}

void closeGripper()
{
  arm.closeGripper();
  delay(500);
}

// "gently" lift the cube upwards at first and then move backwards
void liftUpAndBack()
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.height = 0;
  position = arm.moveGripperTo(position);
  delay(500);
  
  position.moveBackward(5.0);
  arm.moveGripperTo(position);
  delay(500);
}


void lowerGripper()
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.moveDown(3.0);
  arm.moveGripperTo(position);
  delay(500);
}

void rotateLeft()
{
  arm.setDirectBaseRotation(-15.0f);
  delay(500);
}

void rotateRight()
{
  arm.setDirectBaseRotation(15.0f);
  delay(500);
}


void setup() {
  Serial.begin(9600);
  arm.setup();
  delay(10);
  arm.returnToRest();
  // the gripper is closed by default - need to open it
  openGripper();
  delay(1000);

  // move towards the top-most cube on the right
  // and wait a bit so one of the humans can position
  // the initial stack of cubes there
  rotateRight();
  moveToCube(4);
  delay(10000);

  // move to the starting position
  liftUpAndBack();
  delay(500);
  arm.returnToRest();
  openGripper();
  delay(2000);
}

void loop()
{  
  // move the stack from the right to the left
  for(int i = 4; i >= 0; i--)
  {
    // take cube i from the right
    rotateRight();
    moveToCube(i);
    lowerGripper();
    closeGripper();
    liftUpAndBack();

    // put cube i to the left
    rotateLeft();
    moveToCube(4 - i);
    lowerGripper();
    openGripper();   
    liftUpAndBack(); 
  }

  delay(500);

  // move the stack back from the left to the rigth
  for(int i = 4; i >= 0; i--)
  {
    // take cube i from the left
    rotateLeft();
    moveToCube(i);
    lowerGripper();
    closeGripper();
    liftUpAndBack();

    // put cube i to the right
    rotateRight();
    moveToCube(4 - i);
    lowerGripper();
    openGripper();   
    liftUpAndBack(); 
  }
}
