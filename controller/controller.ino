#include <WiFi.h>
#include "include/motorController.h"
#include "include/ultrasonicSensor.h"

const char* ssid = "honor";
const char* password = "";

WiFiServer server(8080);
motorController motors;
DualUltrasonicController ultrasonic;

TaskHandle_t ultrasonicTaskHandle;

void ultrasonicTask(void* pvParameters) {
  for (;;) {
    String status = ultrasonic.getObstacleStatus();
    Serial.println("Obstacle Status: " + status);

    ultrasonic.avoidObstacles(motors);

    vTaskDelay(200 / portTICK_PERIOD_MS); 
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());
  
  server.begin();
  motors.init();
  ultrasonic.init();
  motors.calibrateESCs();
  
  xTaskCreatePinnedToCore(ultrasonicTask, "Ultrasonic Task", 4096, NULL, 1, &ultrasonicTaskHandle, 1);
  
  Serial.println("ESP32 TCP Motor Controller Ready");
}

void loop() {        
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("Client connected");
    String inputString = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' || c == '\r') {
          inputString.trim();
          
          Serial.print("Received command: '");
          Serial.print(inputString);
          Serial.print("' (Length: ");
          Serial.print(inputString.length());
          Serial.println(")");
          
          Serial.print("ASCII values: ");
          for (int i = 0; i < inputString.length(); i++) {
            Serial.print((int)inputString[i]);
            Serial.print(" ");
          }
          Serial.println();
          
          if (inputString.equals("FORWARD")) {
            Serial.println("Executing: FORWARD");
            motors.moveForward();
          }
          else if (inputString.equals("TURN_LEFT")) {
            Serial.println("Executing: TURN_LEFT");
            motors.turnLeft();
          }
          else if (inputString.equals("TURN_RIGHT")) {
            Serial.println("Executing: TURN_RIGHT");
            motors.turnRight();
          }
          else if (inputString.equals("STOP")) {
            Serial.println("Executing: STOP");
            motors.stopMotors();
          }
          else if (inputString.length() == 1 && isDigit(inputString[0])) {
            Serial.println("Setting speed of the motors");
            int speed = map(inputString[0] - '0', 0, 9, 0, 90);
            Serial.print("Speed set to: ");
            Serial.println(speed);
            motors.setSpeed(speed);
          }
          else {
            Serial.println("Unknown command: '" + inputString + "'");
            
            inputString.toUpperCase();
            if (inputString.equals("FORWARD")) {
              Serial.println("Executing: FORWARD (case corrected)");
              motors.moveForward();
            }
            else if (inputString.equals("TURN_LEFT")) {
              Serial.println("Executing: TURN_LEFT (case corrected)");
              motors.turnLeft();
            }
            else if (inputString.equals("TURN_RIGHT")) {
              Serial.println("Executing: TURN_RIGHT (case corrected)");
              motors.turnRight();
            }
            else if (inputString.equals("STOP")) {
              Serial.println("Executing: STOP (case corrected)");
              motors.stopMotors();
            }
          }
          
          client.println("OK");
          inputString = "";
        } else if (c != '\r') { 
          inputString += c;
        }
      }
    }
    
    client.stop();
    Serial.println("Client disconnected");
  }
}
