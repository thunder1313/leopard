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
  digitalWrite(LED_PIN, LOW);  // LED off initially

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connect to the web server at: http://");
  Serial.println(WiFi.softAPIP());

  // Define a single POST route `/request` to control the LED based on code
  server.on("/request", HTTP_POST, []() {  
    if (server.hasArg("plain")) {  // Check if there's a request body
      String postData = server.arg("plain");

      if (postData == "100") {  // If the argument is "100", turn LED on
        digitalWrite(LED_PIN, LOW);  // Set LED on (assuming LOW is ON)
        server.send(200, "text/plain", "LED is ON");
      } else if (postData == "200") {  // If the argument is "200", turn LED off
        digitalWrite(LED_PIN, HIGH);  // Set LED off (assuming HIGH is OFF)
        server.send(200, "text/plain", "LED is OFF");
      } else {
        server.send(400, "text/plain", "Bad Request: Invalid argument");
      }
    } else {
      server.send(400, "text/plain", "Bad Request: No argument provided");
    }
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
  server.handleClient();
}
