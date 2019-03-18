#include "RobotArm.h"

RobotTools::RobotArm arm;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  Serial.println("EEZYBotarm MK2 test!;\n");
  arm.setup();
  delay(10);
  //arm.returnToRest();
}

const int maxInputSize = 100;
char input[maxInputSize];
int offset = 0;


bool processMessage(char* keyword, float valueA, float valueB, float valueC, float valueD, RobotTools::GripperPosition& out) {
  if(strcmp(keyword, "GET") == 0) {
    String s = "RETURN printout:\n";
    s += String("   ") + out.rotation + "\n";
    s += String("   ") + out.distance + "\n";
    s += String("   ") + out.height + ";\n";
    Serial.write(s.c_str());
    return false;
  } else if(strcmp(keyword, "SET-DIRECT") == 0) {
    arm.setDirectBaseRotation(valueA);
    arm.setDirectMainArmRotation(valueB);
    arm.setDirectSmallArmRotation(valueC);
    arm.setDirectGripperRotation(valueD);
  } else if(strcmp(keyword, "SET-POSITION") == 0) {
    out.rotation = valueA;
    out.distance = valueB;
    out.height = valueC;
    out.gripper = valueD;  
  }
  else {
    Serial.write((String("Unknown command: ") + keyword + ";\n").c_str());
    return false;
  }
  return true;
}

bool readNextToken(char* start, char*& next, char separator) {
  char* tokenEnd = strchr(start, separator);
  if (tokenEnd != 0)
  {
      *tokenEnd = 0;
      next = tokenEnd + 1;
      return true;
  }
  return false;
}

bool readLine(char* line, RobotTools::GripperPosition& out) {
  char* keyword = line;
  char* valueA = 0;
  char* valueB = 0;
  char* valueC = 0;
  char* valueD = 0;
  char* tmp = 0;
  if(readNextToken(keyword, valueA, ' ') 
    && readNextToken(valueA, valueB, ' ')
    && readNextToken(valueB, valueC, ' ')
    && readNextToken(valueC, valueD, ' '))
  {
    return processMessage(keyword, atof(valueA), atof(valueB), atof(valueC), atof(valueD), out);
  }

  return false;
}

bool readSerialInput(RobotTools::GripperPosition& out)
{
  if(!Serial.available())
    return false;

  byte size = Serial.readBytes(input + offset, maxInputSize - 1 - offset);
  input[size + offset] = 0;

  if(maxInputSize - 1 - offset <= 0) {
    Serial.write("Buffer overflow!;\n");
    Serial.write((String("Buffer: \"") + input + "\";\n").c_str());
  }

  bool result = false;
  char* current = input;
  char* next = 0;
  while(readNextToken(current, next, ';')) {
    result = result || readLine(current, out);
    current = next;
  }

  char* bufferEnd = input + (size + offset); 
  if(current < bufferEnd) {
    offset = bufferEnd - current;
    memmove(input, current, offset);
  } else {
    offset = 0;
  }
    
  return result;
}

void loop() {
  RobotTools::GripperPosition newPosition = arm.getCurrentPosition();
  if(readSerialInput(newPosition))
  {
    arm.moveGripperTo(newPosition);
  }
} 
