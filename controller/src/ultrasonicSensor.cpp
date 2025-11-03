#include "include/ultrasonicSensor.h"

UltrasonicSensor::UltrasonicSensor(int trig, int echo)
  : trigPin(trig), echoPin(echo) {}

void UltrasonicSensor::init() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

long UltrasonicSensor::getDistance() {
  const int samples = 1;
  long total = 0;
  int count = 0;

  while (count < samples) {
    digitalWrite(trigPin, LOW);
    delay(2);
    digitalWrite(trigPin, HIGH);
    delay(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000); 
    long distance = duration * 0.017; 

    if (distance > 0) { 
      total += distance;
      count++;
    }

    delay(50); 
  }

  long averageDistance = total / samples;

  Serial.println("Distance: " + String(averageDistance) + " cm");

  return averageDistance;
}
DualUltrasonicController::DualUltrasonicController(int leftTrig, int leftEcho,
                                                   int rightTrig, int rightEcho)
  : leftSensor(leftTrig, leftEcho), rightSensor(rightTrig, rightEcho) {}

void DualUltrasonicController::init() {
  leftSensor.init();
  rightSensor.init();
}

long DualUltrasonicController::getLeftDistance() {
  return leftSensor.getDistance();
}

long DualUltrasonicController::getRightDistance() {
  return rightSensor.getDistance();
}

bool DualUltrasonicController::isLeftObstacle() {
  return getLeftDistance() < 30;  
}

bool DualUltrasonicController::isRightObstacle() {
  return getRightDistance() < 30; 
}

String DualUltrasonicController::getObstacleStatus() {
  if (isLeftObstacle() && isRightObstacle()) return "BOTH OBSTACLES";
  if (isLeftObstacle()) return "LEFT OBSTACLE";
  if (isRightObstacle()) return "RIGHT OBSTACLE";
  return "CLEAR";
}

void DualUltrasonicController::avoidObstacles(motorController& motors) {
  if (isLeftObstacle() && isRightObstacle()) {
    motors.stopMotors();
  } else if (isLeftObstacle()) {
    motors.turnRight();
  } else if (isRightObstacle()) {
    motors.turnLeft();
  }
}
