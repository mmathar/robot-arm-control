# robot-arm-control
Custom (non-official) code for having fun with the EEZYbotARM MK2. 

## Youtube-Video (click)
[![Watch the video](https://img.youtube.com/vi/LWbK3SZWLdg/hqdefault.jpg)](https://youtu.be/LWbK3SZWLdg)

## Hardware used
* [EEZYbotARM MK2 by daGHIZmo](http://www.eezyrobots.it/eba_mk2.html)
* [Modified V-gripper by Runkel](https://www.thingiverse.com/thing:2414264) (original: [Vector concept gripper by RubisTechnologies](https://www.thingiverse.com/thing:66818/makes))
* [Customizable PCB Standoff Spacer](https://www.thingiverse.com/thing:1360337) (scaled up)
* Printed on an Ender 3
* Arduino Uno
* Adafruit PCA9685 16-Channel Servo Driver (wired up like [this](https://learn.adafruit.com/16-channel-pwm-servo-driver/using-the-adafruit-library))
* 4x [TowerPro MG996R Servo](https://servodatabase.com/servo/towerpro/mg996r)

## Directory structure
1. **ArduinoSketches**: *.ino sketch files (for upload to the Arduino)
2. **RobotArmLibrary**: Arduino library which simplifies sending commands to the robot
3. **UserInterface**: Java application which can control the robot arm (using UI sliders or a XBox controller) via the serial connection. (Provided the corresponding sketch in ArduinoSketches/Controller is loaded on the microcontroller.)

## Can I use this?
Feel free to use parts/all of the code as you wish. One word of warning though: The code may or may not work for you because it is calibrated for my specific hardware. Using it with different servos requires re-calibration. For that please take a look at _RobotArmLibrary/RobotArm.cpp_ (specifically the constructor and setup method). The IDs for the servos and the minimum/maximum PWM values probably need to be recalibrated.

Also, only a XBox One controller was tested - no idea if others would work.
