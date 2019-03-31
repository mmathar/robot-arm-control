Directory structure:
1. ArduinoSketches contains the *.ino sketch files (to be uploaded to the Arduino)
2. CustomEEZYbotarmControl contains an Arduino library with the common functionality shared between the sketches in (1.).
3. UserInterface contains a Java application which can control the robot arm (using UI or a XBox controller) via the serial connection. (Provided the corresponding sketch in ArduinoSketches/RemoteControl is loaded on the microcontroller.)