#include <RobotArm.h>

RobotTools::RobotArm arm;

void setup() {
  Serial.begin(9600);
  arm.setup();
  delay(10);
  arm.returnToRest();
}

// move to grab cube from stack (index 0 is the bottom of the stack
void moveToCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.moveForward(5.0);
  position.moveDown(11.0 - index * 1.7f);
  position = arm.moveGripperTo(position);
  delay(500);
  position.moveDown(1.0);
  position.moveForward(1.0);
  position = arm.moveGripperTo(position);
  delay(500);
}

void moveBackFromCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.moveUp(1.0);
  position.moveBackward(1.0);
  position = arm.moveGripperTo(position);
  delay(500);
  position.moveBackward(5.0);
  position.moveUp(11.0 - index * 1.7f);
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

void loop()
{ 
  arm.setDirectBaseRotation(10.0f);
  moveToCube(4);
  delay(10000);

  arm.returnToRest();

  delay(2000);

  for(int j = 0; j < 1000; j++)
  {  
  for(int i = 0; i < 5; i++)
  {
    arm.setDirectBaseRotation(10.0f);
    delay(500);
    moveToCube(4 - i);
    closeGripper();  
    moveBackFromCube(4 - i);

    arm.setDirectBaseRotation(-10.0f);
    delay(500);
    moveToCube(i);
    openGripper();
    moveBackFromCube(i);

    arm.setDirectSmallArmRotation(90.0f);
    arm.setDirectMainArmRotation(0.0f);
  }
  
  delay(1000);
  arm.returnToRest();
  delay(1000);

  for(int i = 0; i < 5; i++)
  {
    arm.setDirectBaseRotation(-10.0f);
    delay(500);
    moveToCube(4 - i);
    closeGripper();  
    moveBackFromCube(4 - i);
  
    arm.setDirectBaseRotation(10.0f);
    delay(500);
    moveToCube(i);
    openGripper();
    moveBackFromCube(i);

    arm.setDirectSmallArmRotation(90.0f);
    arm.setDirectMainArmRotation(0.0f);
  }

  delay(1000);
  arm.returnToRest();
  delay(1000);
}


  delay(5000);
}

/*
// move to grab cube from stack (index 0 is the bottom of the stack
void moveToCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.moveForward(5.0);
  position.moveDown(10.7 - index * 2.2f);
  position = arm.moveGripperTo(position);
  delay(500);
  position.moveDown(1.0);
  position.moveForward(1.0);
  position = arm.moveGripperTo(position);
  delay(500);
}

void moveBackFromCube(int index)
{
  RobotTools::GripperPosition position = arm.getCurrentPosition();
  position.moveUp(1.0);
  position.moveBackward(1.0);
  position = arm.moveGripperTo(position);
  delay(500);
  position.moveBackward(5.0);
  position.moveUp(10.7 - index * 2.2f);
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

void loop()
{ 
  arm.setDirectBaseRotation(10.0f);
  moveToCube(4);
  delay(10000);

  arm.returnToRest();

  delay(2000);

  for(int j = 0; j < 1000; j++)
  {  
  for(int i = 0; i < 5; i++)
  {
    arm.setDirectBaseRotation(10.0f);
    delay(500);
    moveToCube(4 - i);
    closeGripper();  
    moveBackFromCube(4 - i);

    arm.setDirectBaseRotation(-10.0f);
    delay(500);
    moveToCube(i);
    openGripper();
    moveBackFromCube(i);

    arm.setDirectSmallArmRotation(90.0f);
    arm.setDirectMainArmRotation(0.0f);
  }
  
  delay(1000);
  arm.returnToRest();
  delay(1000);

  for(int i = 0; i < 5; i++)
  {
    arm.setDirectBaseRotation(-10.0f);
    delay(500);
    moveToCube(4 - i);
    closeGripper();  
    moveBackFromCube(4 - i);
  
    arm.setDirectBaseRotation(10.0f);
    delay(500);
    moveToCube(i);
    openGripper();
    moveBackFromCube(i);

    arm.setDirectSmallArmRotation(90.0f);
    arm.setDirectMainArmRotation(0.0f);
  }

  delay(1000);
  arm.returnToRest();
  delay(1000);
}


  delay(5000);
}*/
