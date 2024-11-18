#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

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
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
      server.send(400, "text/plain", "Invalid JSON");
      return;
    }
    
    String operation = doc["operation"];
    int pulseWidth = 1500;  //default value
  
    if (operation == "moveForward") {
      pulseWidth = 1900;
      pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Moving forward: " + String(pulseWidth) + " µs");
    }
    else if (operation == "moveBack") {
      pulseWidth = 1100;
      pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Moving backward: " + String(pulseWidth) + " µs");
    }
    else if (operation == "turnLeft") {
      pulseWidth = 1100;
      pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turning left: " + String(pulseWidth) + " µs");
    }
    else if (operation == "turnRight") {
      pulseWidth = 1900;
      pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turning right: " + String(pulseWidth) + " µs");
    }
    else if (operation == "turretLeft") {
      pulseWidth = 1100;
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret to the left: " + String(pulseWidth) + " µs");
    }
    else if (operation == "turretRight") {
      pulseWidth = 1900;
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret to the right: " + String(pulseWidth) + " µs");
    }
    else if (operation == "turretOnOff") {
      pulseWidth = 1000;  
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret on/off: " + String(pulseWidth) + " µs");
    }
    else if (operation == "cannonUp") {
      pulseWidth = 1100;  
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon up: " + String(pulseWidth) + " µs");
    }
    else if (operation == "cannonDown") {
      pulseWidth = 1900;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon down: " + String(pulseWidth) + " µs");
    }
    else if (operation == "cannonShot") {
      pulseWidth = 2000;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon shot: " + String(pulseWidth) + " µs");
    }
    else if (operation == "machineGunShot") {
      pulseWidth = 1000;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Machine gun shot: " + String(pulseWidth) + " µs");
    }
    else if (operation == "cannonOnOff") {
      pulseWidth = 1000;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon on/off: " + String(pulseWidth) + " µs");
    }
    else {
      server.send(400, "text/plain", "Invalid operation");
    }
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop() {
  server.handleClient();
}
