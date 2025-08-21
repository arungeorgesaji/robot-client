#include "include/motorController.h"

motorController motors;

string inputString = "";
bool stringComplete = false;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  motors.init();
  motors.calibrateESCs();
  
  Serial.println("ESP32 L298N Motor Control Initialized");
}

void loop() {
  char command = ' ';
  if (Serial2.available()) {
    command = Serial2.read();
  }
  
  switch(command) {
    case 'FORWARD': 
      motors.moveForward();
      Serial.println("Moving Forward");
      break;
    case 'BACKWARD': 
      motors.moveBackward();
      Serial.println("Moving Backward");
      break;
    case 'TURN_LEFT': 
      motors.turnLeft();
      Serial.println("Turning Left");
      break;
    case 'TURN_RIGHT': 
      motors.turnRight();
      Serial.println("Turning Right");
      break;
    case '0'...'9':
      int speed = map(command - '0', 0, 9, 0, 255);
      motors.setSpeed(speed);
      Serial.print("Speed set to: ");
      Serial.println(speed);
      break;
  }
}
