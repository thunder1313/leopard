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
  int pulseWidth = DEFAULT_PULSE;

  switch (code) {
    case 101: // moveForward 1600-1900 od lekko do pelna wixa
      pulseWidth = 1900;
      pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Moving forward: " + String(pulseWidth) + " µs");
      break;
          
    case 102: // moveBack 1400-1100 wolno do szybko
      pulseWidth = 1100;
      pwm.writeMicroseconds(THROTTLE_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Moving backward: " + String(pulseWidth) + " µs");
      break;

    case 103: // turnLeft 1350-900 
      pulseWidth = 1100;
      pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turning left: " + String(pulseWidth) + " µs");
      break;

    case 104: // turnRight 1600-2000
      pulseWidth = 1900;
      pwm.writeMicroseconds(ROTATION_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turning right: " + String(pulseWidth) + " µs");
      break;

    default:
      server.send(400, "text/plain", "Invalid operation code for Driver");
      break;
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
  int pulseWidth = DEFAULT_PULSE;

  switch (code) {
    case 105: // turretLeft 1300-1100 od lekko, do wixa w lewo 
      pulseWidth = 1100;
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret to the left: " + String(pulseWidth) + " µs");
      break;

    case 106: // turretRight 1600-2000 wolno do szybko w wiezy 
      pulseWidth = 2000; 
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret to the right: " + String(pulseWidth) + " µs");
      break;

    case 107: // turretOnOff
      pulseWidth = 1000;  
      pwm.writeMicroseconds(TURRET_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Turret on/off: " + String(pulseWidth) + " µs");
      break;

    default:
      server.send(400, "text/plain", "Invalid operation code for Commander");
      break;
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
  int pulseWidth = DEFAULT_PULSE;

  switch (code) {
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
      pulseWidth = 2100;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon shot: " + String(pulseWidth) + " µs");
      delay(500);
      pwm.writeMicroseconds(CANNON_CHANNEL, DEFAULT_PULSE);
      break;

    case 111: // machineGunShot
      pulseWidth = 900;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Machine gun shot: " + String(pulseWidth) + " µs");
      delay(500);
      pwm.writeMicroseconds(CANNON_CHANNEL, DEFAULT_PULSE);
      break;

    case 112: // cannonOnOff
      pulseWidth = 1000;
      pwm.writeMicroseconds(CANNON_CHANNEL, pulseWidth);
      server.send(200, "text/plain", "Cannon on/off: " + String(pulseWidth) + " µs");
      break;

    default:
      server.send(400, "text/plain", "Invalid operation code for Gunner");
      break;
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