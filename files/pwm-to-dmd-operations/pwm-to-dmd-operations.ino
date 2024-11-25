#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <map>

Adafruit_PWMServoDriver pwm;

#define THROTTLE_CHANNEL 0
#define ROTATION_CHANNEL 1
#define TURRET_CHANNEL 2
#define CANNON_CHANNEL 3

#define PCA9685_ADDR 0x40  // Default I2C address of PCA9685

const char* ssid = "Leopard_2A6";
const char* password = "superczolg";
ESP8266WebServer server(80);

// Helper function to add CORS headers
void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With");
}

// Handle preflight OPTIONS request for CORS
void handleCORSOptions() {
  addCORSHeaders();
  server.send(204);  // No Content
}

std::map<int, String> codeToOperation = {
    {101, "moveForward"},
    {102, "moveBack"},
    {103, "turnLeft"},
    {104, "turnRight"},
    {105, "turretLeft"},
    {106, "turretRight"},
    {107, "turretOnOff"},
    {108, "cannonUp"},
    {109, "cannonDown"},
    {110, "cannonShot"},
    {111, "machineGunShot"},
    {112, "cannonOnOff"}
};

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1);  // Initialize I2C (SDA, SCL)

  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(50);  //default PWM frequency - 50 Hz

  WiFi.softAP(ssid, password);
  Serial.println("Connect to the web server at: http://" + WiFi.softAPIP().toString());

  server.on("/request", HTTP_OPTIONS, handleCORSOptions);

  server.on("/request", HTTP_POST, []() {
    addCORSHeaders();

    if (server.hasArg("plain")) {
      String postData = server.arg("plain");
      StaticJsonDocument<200> jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, postData);

      if (error) {
        server.send(400, "text/plain", "Bad Request: Invalid JSON format");
        return;
      }

      int code = jsonDoc["code"] | -1;
      int pulseWidth = 1500;  //default value
      String operation = codeToOperation[code];

      if (operation == "") {
        server.send(400, "text/plain", "Invalid operation code");
        return;
      }

      switch (code) {
        case 101: // moveForward
          pulseWidth = 1900;
          pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Moving forward: " + String(pulseWidth) + " µs");
          break;
          
        case 102: // moveBack
          pulseWidth = 1100;
          pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Moving backward: " + String(pulseWidth) + " µs");
          break;

        case 103: // turnLeft
          pulseWidth = 1100;
          pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Turning left: " + String(pulseWidth) + " µs");
          break;

        case 104: // turnRight
          pulseWidth = 1900;
          pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Turning right: " + String(pulseWidth) + " µs");
          break;

        case 105: // turretLeft
          pulseWidth = 1100;
          pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Turret to the left: " + String(pulseWidth) + " µs");
          break;

        case 106: // turretRight
          pulseWidth = 1900;
          pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Turret to the right: " + String(pulseWidth) + " µs");
          break;

        case 107: // turretOnOff
          pulseWidth = 1000;  
          pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Turret on/off: " + String(pulseWidth) + " µs");
          break;

        case 108: // cannonUp
          pulseWidth = 1100;  
          pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Cannon up: " + String(pulseWidth) + " µs");
          break;

        case 109: // cannonDown
          pulseWidth = 1900;
          pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Cannon down: " + String(pulseWidth) + " µs");
          break;

        case 110: // cannonShot
          pulseWidth = 2000;
          pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Cannon shot: " + String(pulseWidth) + " µs");
          break;

        case 111: // machineGunShot
          pulseWidth = 1000;
          pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Machine gun shot: " + String(pulseWidth) + " µs");
          break;

        case 112: // cannonOnOff
          pulseWidth = 1000;
          pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
          server.send(200, "text/plain", "Cannon on/off: " + String(pulseWidth) + " µs");
          break;

        default:
          server.send(400, "text/plain", "Invalid operation code");
          break;
      }
    }
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}
