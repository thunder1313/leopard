#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <map>

// Initialize the servo driver
Adafruit_PWMServoDriver pwm;

#define THROTTLE_CHANNEL 0
#define ROTATION_CHANNEL 1
#define TURRET_CHANNEL 2
#define CANNON_CHANNEL 3

#define PCA9685_ADDR 0x40  // Default I2C address of PCA9685
#define DEFAULT_PULSE 1500

#define LED_PIN LED_BUILTIN

const char* ssid = "Leopard_2A6";
const char* password = "superczolg";
ESP8266WebServer server(80);

void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With");
}

void handleCORSOptions() {
  addCORSHeaders();
  server.send(204);  // No Content
}

void reset() {
  pwm.writeMicroseconds(THROTTLE_CHANNEL, DEFAULT_PULSE);
  pwm.writeMicroseconds(ROTATION_CHANNEL, DEFAULT_PULSE);
  pwm.writeMicroseconds(TURRET_CHANNEL, DEFAULT_PULSE);
  pwm.writeMicroseconds(CANNON_CHANNEL, DEFAULT_PULSE);
  Serial.println("Reset all signals");
}

void killAllSignals() {
  addCORSHeaders();
  reset();
  server.send(200, "text/plain", "All signals reset to default pulse width");
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

void cancelDriver() {
  addCORSHeaders();
  pwm.writeMicroseconds(THROTTLE_CHANNEL, DEFAULT_PULSE);
  pwm.writeMicroseconds(ROTATION_CHANNEL, DEFAULT_PULSE);
  server.send(200, "text/plain", "Driver actions canceled");
}

void cancelCommander() {
  addCORSHeaders();
  pwm.writeMicroseconds(TURRET_CHANNEL, DEFAULT_PULSE);
  server.send(200, "text/plain", "Commander actions canceled");
}

void cancelGunner() {
  addCORSHeaders();
  pwm.writeMicroseconds(CANNON_CHANNEL, DEFAULT_PULSE);
  server.send(200, "text/plain", "Gunner actions canceled");
}

void handleDriver() {
  addCORSHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request: Missing payload");
    return;
  }

  String postData = server.arg("plain");
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, postData);

  if (error) {
    server.send(400, "text/plain", "Bad Request: Invalid JSON format");
    return;
  }

  int code = jsonDoc["code"] | -1;
  if (code == 101 || code == 102 || code == 103 || code == 104) {
    int pulseWidth = (code == 101) ? 1900 : (code == 102) ? 1100 : (code == 103) ? 1100 : 1900;
    pwm.writeMicroseconds((code <= 102) ? THROTTLE_CHANNEL : ROTATION_CHANNEL, pulseWidth);
    server.send(200, "text/plain", "Driver action executed: " + codeToOperation[code]);
  } else {
    server.send(400, "text/plain", "Invalid operation for Driver");
  }
}

void handleCommander() {
  addCORSHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request: Missing payload");
    return;
  }

  String postData = server.arg("plain");
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, postData);

  if (error) {
    server.send(400, "text/plain", "Bad Request: Invalid JSON format");
    return;
  }

  int code = jsonDoc["code"] | -1;
  if (code == 105 || code == 106 || code == 107) {
    int pulseWidth = (code == 105) ? 1100 : (code == 106) ? 2000 : 1000;
    pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
    server.send(200, "text/plain", "Commander action executed: " + codeToOperation[code]);
  } else {
    server.send(400, "text/plain", "Invalid operation for Commander");
  }
}

void handleGunner() {
  addCORSHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request: Missing payload");
    return;
  }

  String postData = server.arg("plain");
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, postData);

  if (error) {
    server.send(400, "text/plain", "Bad Request: Invalid JSON format");
    return;
  }

  int code = jsonDoc["code"] | -1;
  if (code == 108 || code == 109 || code == 110 || code == 111 || code == 112) {
    int pulseWidth = (code == 108) ? 1100 : (code == 109) ? 1900 : (code == 110) ? 2100 : (code == 111) ? 900 : 1000;
    pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
    server.send(200, "text/plain", "Gunner action executed: " + codeToOperation[code]);
    pwm.writeMicroseconds(CANNON_CHANNEL, DEFAULT_PULSE);
  } else {
    server.send(400, "text/plain", "Invalid operation for Gunner");
  }
}

void handleLoader() {
  addCORSHeaders();

  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Bad Request: Missing payload");
    return;
  }

  String postData = server.arg("plain");
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, postData);

  if (error) {
    server.send(400, "text/plain", "Bad Request: Invalid JSON format");
    return;
  }

  int code = jsonDoc["code"] | -1;
  //TODO
}

void handlePing() {
  addCORSHeaders();
  digitalWrite(LED_PIN, LOW);
  server.send(200, "text/plain", "pong");
  delay(500);
  digitalWrite(LED_PIN, HIGH);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);
  Wire.begin(D2, D1);

  pwm.begin();
  pwm.setPWMFreq(50);
  reset();

  WiFi.softAP(ssid, password);
  Serial.println("Web server started at: http://" + WiFi.softAPIP().toString());

  server.on("/driver", HTTP_OPTIONS, handleCORSOptions);
  server.on("/driver", HTTP_POST, handleDriver);

  server.on("/commander", HTTP_OPTIONS, handleCORSOptions);
  server.on("/commander", HTTP_POST, handleCommander);

  server.on("/gunner", HTTP_OPTIONS, handleCORSOptions);
  server.on("/gunner", HTTP_POST, handleGunner);

  server.on("/loader", HTTP_OPTIONS, handleCORSOptions);
  server.on("/loader", HTTP_POST, handleLoader);

  server.on("/driver/cancel", HTTP_POST, cancelDriver);
  server.on("/commander/cancel", HTTP_POST, cancelCommander);
  server.on("/gunner/cancel", HTTP_POST, cancelGunner);

  server.on("/kill", HTTP_POST, killAllSignals);

  server.on("/ping", HTTP_GET, handlePing);

  server.begin();
}

void loop() {
  server.handleClient();
}
