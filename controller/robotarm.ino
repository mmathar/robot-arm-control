#include "RobotArm.h"
#include "SerialConnection.h"

RobotTools::RobotArm arm;
RobotTools::SerialConnection connection;

void setup() {
  connection.setup();
  arm.setup();
  delay(10);
  //arm.returnToRest();
}

// handle requests for parameter values (just return a printout)
void processGet()
{
    RobotTools::GripperPosition position = arm.getCurrentPosition();
    String s = "RETURN printout:\n";
    s += String("   ") + position.rotation + "\n";
    s += String("   ") + position.distance + "\n";
    s += String("   ") + position.height + " \n";
    s += String("   ") + position.gripper + ";\n";
    Serial.write(s.c_str());
}

// the remote party wants the arm to move the gripper to a new specified position
void processSetPosition(RobotTools::Line& line)
{
    if(line.countParts() != 4)
        return;

    RobotTools::GripperPosition newPosition;
    newPosition.rotation = line.parseFloat();
    newPosition.distance = line.parseFloat();
    newPosition.height = line.parseFloat();
    newPosition.gripper = line.parseFloat(); 
    arm.moveGripperTo(newPosition);
}

// the remote party wants to update the gripper position by a specified delta
void processUpdatePosition(RobotTools::Line& line)
{
    if(line.countParts() != 4)
        return;

    RobotTools::GripperPosition position = arm.getCurrentPosition();
    position.rotation += line.parseFloat();
    position.distance += line.parseFloat();
    position.height += line.parseFloat();
    position.gripper += line.parseFloat(); 
    arm.moveGripperTo(position);
}

// the remote party wants the servos to assume the positions described by these angles
void processSetAngles(RobotTools::Line& line)
{
    if(line.countParts() != 4)
        return;

    arm.setDirectBaseRotation(line.parseFloat());
    arm.setDirectMainArmRotation(line.parseFloat());
    arm.setDirectSmallArmRotation(line.parseFloat());
    arm.setDirectGripperRotation(line.parseFloat());
}

// handles the different message types that may be received
void processMessage(RobotTools::Line& line)
{
    if(line.isEmpty())
        return;

    const char* keyword = line.parseString();
    if(strcmp(keyword, "GET") == 0) 
    {
       processGet();
    } 
    else if(strcmp(keyword, "SET-DIRECT") == 0) 
    {
        processSetAngles(line);
    } 
    else if(strcmp(keyword, "SET-POSITION") == 0) 
    {
       processSetPosition(line); 
    }
    else if(strcmp(keyword, "UPDATE-POSITION") == 0) 
    {
       processSetPosition(line); 
    }
    else 
    {
        Serial.write((String("Unknown command: ") + keyword + ";\n").c_str());
    }
}

void readCommandsFromSerial()
{
    connection.updateBuffer();
    while(RobotTools::Line line = connection.readLine())
    {
        processMessage(line);
    }
}

void loop() 
{
    readCommandsFromSerial();
} 
