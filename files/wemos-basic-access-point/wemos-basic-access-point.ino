#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define LED_PIN LED_BUILTIN

// Define network ssid and password
const char* ssid = "Leopard_2A6";
const char* password = "superczolg";
ESP8266WebServer server(80);   // Instantiate server at port 80 (http port)

void setup(void) {
  // HTML content of the web page for controlling the LED
  String index = "<h1>LED Control Web Server</h1>"
                 "<p><a href=\"/LEDOn\"><button>LED ON</button></a>"
                 "&nbsp;<a href=\"/LEDOff\"><button>LED OFF</button></a></p>";

  // Set up the LED pin mode and turn it on initially
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Turn LED on at the start

  // Set up WiFi access point
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connect to the web server at: http://");
  Serial.println(WiFi.softAPIP());

  // Define web server routes
  server.on("/", [index]() {  // Main page
    server.send(200, "text/html", index);
  });

  server.on("/LEDOn", [index]() {  // Turn LED on
    digitalWrite(LED_PIN, LOW);
    server.send(200, "text/html", index);
  });

  server.on("/LEDOff", [index]() {  // Turn LED off
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/html", index);
  });

  server.begin();
  Serial.println("Web server started!");
}

void loop(void) {
  server.handleClient();
}
