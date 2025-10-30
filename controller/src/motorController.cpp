#include "include/motorController.h"

motorController::motorController(int leftPin, int rightPin) {
  motorLeft.attach(leftPin, 1000, 2000);
  motorRight.attach(rightPin, 1000, 2000);
  
  motorLeft.write(MIN_SPEED);
  motorRight.write(MIN_SPEED);
}

void motorController::init() {
  motorLeft.write(MIN_SPEED);
  motorRight.write(MIN_SPEED);
  Serial.println("Brushless Motors Initialized");

  delay(5000);
}

void motorController::setSpeed(int speed) {
  currentSpeed = constrain(speed, MIN_SPEED, MAX_SPEED);
  
  motorLeft.write(currentSpeed);
  motorRight.write(currentSpeed);
  
  Serial.print("Speed set to: ");
  Serial.println(currentSpeed);
}

void motorController::moveForward() {
  motorLeft.write(currentSpeed);
  motorRight.write(currentSpeed);
  Serial.println("Moving Forward");
}

void motorController::turnLeft() {
  motorLeft.write(MIN_SPEED);
  motorRight.write(currentSpeed);
  Serial.println("Turning Left");
}

void motorController::turnRight() {
  motorLeft.write(currentSpeed);
  motorRight.write(MIN_SPEED);
  Serial.println("Turning Right");
}

void motorController::stopMotors() {
  motorLeft.write(MIN_SPEED);
  motorRight.write(MIN_SPEED);
  Serial.println("Motors Stopped");
}

void motorController::calibrateESCs() {
  Serial.println("Starting ESC calibration...");
  Serial.println("Connect battery now, wait for beeps...");
  
  motorLeft.write(MAX_SPEED);
  motorRight.write(MAX_SPEED);
  delay(2000);
  
  motorLeft.write(MIN_SPEED);
  motorRight.write(MIN_SPEED);
  delay(2000);
  
  motorLeft.write(MIN_SPEED);
  motorRight.write(MIN_SPEED);
  
  Serial.println("Calibration complete!");
}

int motorController::getCurrentSpeed() {
  return currentSpeed;
}
