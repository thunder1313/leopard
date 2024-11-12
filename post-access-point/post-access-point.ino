#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LED_PIN LED_BUILTIN

// Define network SSID and password
const char* ssid = "Leopard_2A6";
const char* password = "superczolg";
ESP8266WebServer server(80);   // Instantiate server at port 80 (HTTP port)

void setup(void) {
  // Set up the LED pin mode and turn it off initially
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED on initially

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connect to the web server at: http://");
  Serial.println(WiFi.softAPIP());

  // Define web server POST routes
  server.on("/LEDOn", HTTP_POST, []() {  // Turn LED on with POST request
    digitalWrite(LED_PIN, LOW);  // Set LED on
    server.send(200, "text/plain", "LED is ON");
  });

  server.on("/LEDOff", HTTP_POST, []() {  // Turn LED off with POST request
    digitalWrite(LED_PIN, HIGH);  // Set LED off
    server.send(200, "text/plain", "LED is OFF");
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
  server.handleClient();
}
