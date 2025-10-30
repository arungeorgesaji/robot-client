#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

#include <ESP32Servo.h>

#define LEFT_MOTOR_PIN 26
#define RIGHT_MOTOR_PIN 13
#define MIN_SPEED 0
#define MAX_SPEED 90

class motorController {
private:
  Servo motorLeft;
  Servo motorRight;
  int currentSpeed;

public:
  motorController(int leftPin = LEFT_MOTOR_PIN, int rightPin = RIGHT_MOTOR_PIN);
  
  void init();
  void setSpeed(int speed);
  void moveForward();
  void turnLeft();
  void turnRight();
  void stopMotors();
  void calibrateESCs();
  int getCurrentSpeed();
};

#endif
