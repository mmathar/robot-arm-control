#include <RobotArm.h>

RobotTools::RobotArm arm;

// move to grab cube from stack (index 0 is the bottom of the stack
void moveToCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.distance = 5.0;
  position.height = 14.7 - 2.4f * index;
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

void putDown()
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
    //putDown();
    closeGripper();
    liftUpAndBack();
    
    rotateLeft();
    moveToCube(4 - i);
    //putDown();
    openGripper();   
    liftUpAndBack(); 
  }

  delay(500);

  // stack to the right
  for(int i = 4; i >= 0; i--)
  {
    rotateLeft();
    moveToCube(i);
    //putDown();
    closeGripper();
    liftUpAndBack();
    
    rotateRight();
    moveToCube(4 - i);
    //putDown();
    openGripper();   
    liftUpAndBack(); 
  }
}
