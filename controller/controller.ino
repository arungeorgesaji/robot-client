#include <WiFi.h>
#include "include/motorController.h"

const char* ssid = "iBall-Baton";
const char* password = "";

WiFiServer server(8080);  
motorController motors;

void setup() {
  Serial.begin(115200);
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
  motors.calibrateESCs();

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
        if (c == '\n') {
          inputString.trim();
          Serial.println("Command: " + inputString);

          if (inputString == "FORWARD") {
            motors.moveForward();
            Serial.println("Forward");
          } 
          else if (inputString == "BACKWARD") {
            motors.moveBackward();
          } 
          else if (inputString == "TURN_LEFT") {
            motors.turnLeft();
          } 
          else if (inputString == "TURN_RIGHT") {
            motors.turnRight();
          } 
          else if (inputString == "STOP") {
            motors.stopMotors();
          }
          else if (inputString.length() == 1 && isDigit(inputString[0])) {
            int speed = map(inputString[0] - '0', 0, 9, 0, 255);
            motors.setSpeed(speed);
          } 
          else {
            Serial.println("Unknown command: " + inputString);
          }

          client.println("OK"); 
          inputString = "";
        } else {
          inputString += c;
        }
      }
    }

    client.stop();
    Serial.println("Client disconnected");
  }
}
