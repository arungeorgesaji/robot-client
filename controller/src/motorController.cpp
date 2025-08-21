#include "motorController.h"
#include <Arduino.h>

motorController::motorController(int leftPin, int rightPin, int minPulse, int maxPulse) 
  : currentSpeed(90), minPulse(minPulse), maxPulse(maxPulse) {
  motorLeft.attach(leftPin, minPulse, maxPulse);
  motorRight.attach(rightPin, minPulse, maxPulse);
}

void motorController::init() {
  motorLeft.writeMicroseconds(1500);
  motorRight.writeMicroseconds(1500);
  delay(2000); 
  currentSpeed = 90;
  Serial.println("Brushless Motors Initialized");
}

void motorController::setSpeed(int speed) {
  int servoSpeed = map(constrain(speed, 0, 255), 0, 255, 0, 180);
  currentSpeed = servoSpeed;
  
  motorLeft.write(servoSpeed);
  motorRight.write(servoSpeed);
  
  Serial.print("Speed set to: ");
  Serial.println(speed);
}

void motorController::moveForward() {
  if (currentSpeed <= 90) {
    setSpeed(map(128, 0, 255, 0, 255)); 
  }
  motorLeft.write(currentSpeed);
  motorRight.write(currentSpeed);
  Serial.println("Moving Forward");
}

void motorController::moveBackward() {
  int reverseSpeed = map(constrain(currentSpeed, 0, 180), 0, 180, 90, 0);
  motorLeft.write(reverseSpeed);
  motorRight.write(reverseSpeed);
  Serial.println("Moving Backward");
}

void motorController::turnLeft() {
  int leftSpeed = max(0, currentSpeed - 40);
  int rightSpeed = min(180, currentSpeed + 20);
  
  motorLeft.write(leftSpeed);
  motorRight.write(rightSpeed);
  Serial.println("Turning Left");
}

void motorController::turnRight() {
  int leftSpeed = min(180, currentSpeed + 20);
  int rightSpeed = max(0, currentSpeed - 40);
  
  motorLeft.write(leftSpeed);
  motorRight.write(rightSpeed);
  Serial.println("Turning Right");
}

void motorController::stopMotors() {
  motorLeft.write(90);
  motorRight.write(90);
  currentSpeed = 90;
  Serial.println("Motors Stopped");
}

void motorController::calibrateESCs() {
  Serial.println("Starting ESC calibration...");
  Serial.println("Connect battery now, wait for beeps...");
  
  motorLeft.write(180);
  motorRight.write(180);
  delay(2000);
  
  motorLeft.write(0);
  motorRight.write(0);
  delay(2000);
  
  motorLeft.write(90);
  motorRight.write(90);
  delay(2000);
  
  Serial.println("Calibration complete!");
}

int motorController::getCurrentSpeed() {
  return map(currentSpeed, 0, 180, 0, 255);
}
