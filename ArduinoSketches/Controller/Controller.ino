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

// handle requests for fetching parameter values (just return a printout)
void processGet()
{
    RobotTools::GripperPosition position = arm.getCurrentPosition();
    String s = "RETURN printout: {";
    s += String("position: ") + position.rotation + ", ";
    s += String("distance: ") + position.distance + ", ";
    s += String("height: ") + position.height + "};";
    Serial.write(s.c_str());
}

// the remote party (Serial port)
// wants the arm to move the gripper to a new specified position
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

    // discretize the gripper values - only need "open" and "close"
    float gripperDelta = line.parseFloat();
    if(gripperDelta < -5.0f) { // 5 is chosen pretty arbitrarily. The sign is what matters.
      arm.closeGripper();
    } else if(gripperDelta > 5.0f) {
      arm.openGripper();
    }
}

// the remote party (Serial port)
// wants to update the gripper position by a specified delta.
// This is used for controlling the robot using an XBOX controller.
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

    // discretize the gripper values - only need "open" and "close"
    float gripperDelta = line.parseFloat();
    if(gripperDelta < -5.0f) { // 5 is chosen pretty arbitrarily. The sign is what matters.
      arm.closeGripper();
    } else if(gripperDelta > 5.0f) {
      arm.openGripper();
    }
}

// The remote party wants the servos to assume the positions described by these angles.
// This is used for controlling the robot from the UI (the sliders).
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
       // GET: return a printout of the robot's current configuration
       processGet();
    } 
    else if(strcmp(keyword, "SET-DIRECT") == 0) 
    {
        // SET-DIRECT: directly set the angles for all servos
        processSetAngles(line);
    } 
    else if(strcmp(keyword, "SET-POSITION") == 0) 
    {
       // SET-POSITION: directly set the gripper position (in ``robot coordinates'')
       processSetPosition(line); 
    }
    else if(strcmp(keyword, "UPDATE-POSITION") == 0) 
    {
       // UPDATE-POSITION: update the current gripper position by a delta (in ``robot coordinates'')
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
