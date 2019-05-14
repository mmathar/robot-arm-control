#include <RobotArm.h>

RobotTools::RobotArm arm;

// move to grab cube from stack (index 0 is the bottom of the stack
void moveToCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.distance = 6.37;
  position.height = 15.0 - 2.5f * index;
  position.height -= 3.0; // initially aim for a bit higher so we can softly put it down
  
  position = arm.moveGripperTo(position);
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

// "gently" lift the cube upwards at first
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
  position = arm.moveGripperTo(position);
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
  openGripper();
  delay(1000);
  rotateRight();
  moveToCube(4);
  delay(10000);
  liftUpAndBack();
  delay(500);
  arm.returnToRest();
  openGripper();
  delay(2000);
}

void loop()
{  
  // stack to the left
  for(int i = 4; i >= 0; i--)
  {
    rotateRight();
    moveToCube(i);
    lowerGripper();
    closeGripper();
    liftUpAndBack();
    
    rotateLeft();
    moveToCube(4 - i);
    lowerGripper();
    openGripper();   
    liftUpAndBack(); 
  }

  delay(500);

  // stack to the right
  for(int i = 4; i >= 0; i--)
  {
    rotateLeft();
    moveToCube(i);
    lowerGripper();
    closeGripper();
    liftUpAndBack();
    
    rotateRight();
    moveToCube(4 - i);
    lowerGripper();
    openGripper();   
    liftUpAndBack(); 
  }
}
