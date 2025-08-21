#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <ESP32Servo.h>

#define LEFT_MOTOR_PIN 26
#define RIGHT_MOTOR_PIN 27

class motorController {
private:
  Servo motorLeft;
  Servo motorRight;
  int currentSpeed;
  int minPulse;
  int maxPulse;

public:
  motorController(int leftPin = LEFT_MOTOR_PIN, int rightPin = RIGHT_MOTOR_PIN, 
                 int minPulse = 1000, int maxPulse = 2000);
  
  void init();
  void setSpeed(int speed);
  void moveForward();
  void moveBackward();
  void turnLeft();
  void turnRight();
  void stopMotors();
  void calibrateESCs();
  int getCurrentSpeed();
};

#endif
