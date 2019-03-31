#include <RobotArm.h>
#include <SerialConnection.h>

RobotTools::RobotArm arm;
RobotTools::SerialConnection connection;

void setup() {
  connection.setup();
  arm.setup();
  delay(10);
  arm.returnToRest();
}

// handle requests for parameter values (just return a printout)
void processGet()
{
    RobotTools::GripperPosition position = arm.getCurrentPosition();
    String s = "RETURN printout:\n";
    s += String("   ") + position.rotation + "\n";
    s += String("   ") + position.distance + "\n";
    s += String("   ") + position.height + " \n";
    Serial.write(s.c_str());
}

// the remote party wants the arm to move the gripper to a new specified position
void processSetPosition(RobotTools::Line& line)
{
    if(line.countParts() != 4)
    {
       connection.sendError("SetPosition line doesn't have 4 parts!");
       return;
    }

    RobotTools::GripperPosition newPosition;
    newPosition.rotation = line.parseFloat();
    newPosition.distance = line.parseFloat();
    newPosition.height = line.parseFloat();
    arm.moveGripperTo(newPosition);

    float gripperDelta = line.parseFloat();
    if(gripperDelta < -5.0f) {
      arm.closeGripper();
    } else if(gripperDelta > 5.0f) {
      arm.openGripper();
    }
}

// the remote party wants to update the gripper position by a specified delta
void processUpdatePosition(RobotTools::Line& line)
{
    if(line.countParts() != 4)
    {
       connection.sendError("UpdatePosition line doesn't have 4 parts!");
       return;
    }

    RobotTools::GripperPosition position = arm.getCurrentPosition();
    position.rotation += line.parseFloat();
    position.distance += line.parseFloat();
    position.height += line.parseFloat();
    arm.moveGripperTo(position);

    float gripperDelta = line.parseFloat();
    if(gripperDelta < -5.0f) {
      arm.closeGripper();
    } else if(gripperDelta > 5.0f) {
      arm.openGripper();
    }
}

// the remote party wants the servos to assume the positions described by these angles
void processSetAngles(RobotTools::Line& line)
{
    if(line.countParts() != 4)
    {
      connection.sendError("SetAngles line doesn't have 4 parts!");
      return;
    }

    arm.setDirectBaseRotation(line.parseFloat());
    arm.setDirectMainArmRotation(line.parseFloat());
    arm.setDirectSmallArmRotation(line.parseFloat());
    arm.setDirectGripperRotation(line.parseFloat());
}

// handles the different message types that may be received
void processMessage(RobotTools::Line& line)
{
    if(line.isEmpty())
    {
        connection.sendError("Line was empty!");
        return;
    }

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
       processUpdatePosition(line); 
    }
    else 
    {
        connection.sendError(String("Unknown command: ") + keyword);
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
