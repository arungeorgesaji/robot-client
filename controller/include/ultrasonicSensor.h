#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>
#include "motorController.h"

#define LEFT_ECHO_PIN 33
#define LEFT_TRIG_PIN 32
#define RIGHT_ECHO_PIN 27
#define RIGHT_TRIG_PIN 25

class UltrasonicSensor {
private:
  int trigPin;
  int echoPin;
  
public:
  UltrasonicSensor(int trig, int echo);
  void init();
  long getDistance();
};

class DualUltrasonicController {
private:
  UltrasonicSensor leftSensor;
  UltrasonicSensor rightSensor;
  
public:
  DualUltrasonicController(int leftTrig = LEFT_TRIG_PIN, int leftEcho = LEFT_ECHO_PIN,
                          int rightTrig = RIGHT_TRIG_PIN, int rightEcho = RIGHT_ECHO_PIN);
  
  void init();
  long getLeftDistance();
  long getRightDistance();
  bool isLeftObstacle();
  bool isRightObstacle();
  String getObstacleStatus();
  void avoidObstacles(motorController& motors);
};

#endif
