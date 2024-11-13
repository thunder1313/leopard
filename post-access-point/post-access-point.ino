#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>  // Include the ArduinoJson library for JSON parsing

#define LED_PIN LED_BUILTIN

// Define network SSID and password
const char* ssid = "Leopard_2A6";
const char* password = "superczolg";
ESP8266WebServer server(80);   // Instantiate server at port 80 (HTTP port)

// Function to add CORS headers to every response
void addCORSHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-Requested-With");
}

// Handle CORS preflight OPTIONS request
void handleCORSOptions() {
  addCORSHeaders();
  server.send(204);  // Send a 204 No Content response for OPTIONS request
}

void setup(void) {
  // Set up the LED pin mode and turn it off initially
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED off initially

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connect to the web server at: http://");
  Serial.println(WiFi.softAPIP());

  // Handle OPTIONS preflight request at /request for CORS
  server.on("/request", HTTP_OPTIONS, handleCORSOptions);

  // Define a single POST route `/request` to control the LED based on multiple codes
  server.on("/request", HTTP_POST, []() {
    addCORSHeaders();  // Add CORS headers for the response

    if (server.hasArg("plain")) {  // Check if there's a request body
      String postData = server.arg("plain");

      // Parse the JSON data
      StaticJsonDocument<200> jsonDoc;  // Adjust size as needed
      DeserializationError error = deserializeJson(jsonDoc, postData);

      if (error) {
        server.send(400, "text/plain", "Bad Request: Invalid JSON format");
        return;
      }

      if (jsonDoc.containsKey("code1") && jsonDoc["code1"] == 100) {
        digitalWrite(LED_PIN, LOW);  // Turn LED on
        server.send(200, "text/plain", "LED turned on");
      } else if (jsonDoc.containsKey("code2") && jsonDoc["code2"] == 200) {
        digitalWrite(LED_PIN, HIGH);  // Turn LED off
        server.send(200, "text/plain", "LED turned off");
      } else {
        server.send(400, "text/plain", "Bad Request: Missing or invalid code");
      }

    } else {
      server.send(400, "text/plain", "Bad Request: No data provided");
    }
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
  server.handleClient();
}
