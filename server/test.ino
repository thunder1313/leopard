#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#define LED_PIN D0
const char *ssid = "ESP8266-AP";
const char *password = "password";

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

bool isGunLoaded = false;
bool isCanonLoaded = false;
int connectedClients = 0;

void handleRoot() {
  server.send(200, "text/plain", "Successful response.");
}

void broadcast(String message) {
  for (uint8_t i = 0; i < webSocket.connectedClients(); i++) {
    if (webSocket.status(i) == WS_CONNECTED) {
      webSocket.sendTXT(i, message);
    }
  }
}

void handleWebSocketMessage(uint8_t num, uint8_t *payload, size_t length) {
  String message = String((char *)payload);
  Serial.println("Received: " + message);

  DynamicJsonDocument doc(256);
  deserializeJson(doc, message);

  String type = doc["type"];

  if (type == "getIsCanonLoaded") {
    webSocket.sendTXT(num, "{\"type\": \"isCanonLoaded\", \"isCanonLoaded\": " + String(isCanonLoaded) + "}");
  } else if (type == "getIsGunLoaded") {
    webSocket.sendTXT(num, "{\"type\": \"isGunLoaded\", \"isGunLoaded\": " + String(isGunLoaded) + "}");
  } else if (type == "setIsCanonLoaded") {
    isCanonLoaded = !isCanonLoaded;
    broadcast("{\"type\": \"isCanonLoaded\", \"isCanonLoaded\": " + String(isCanonLoaded) + "}");
  } else if (type == "setIsGunLoaded") {
    isGunLoaded = !isGunLoaded;
    broadcast("{\"type\": \"isGunLoaded\", \"isGunLoaded\": " + String(isGunLoaded) + "}");
  }
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      connectedClients++;
      Serial.printf("Client [%u] connected. Total clients: %d\n", num, connectedClients);
      break;
    case WStype_DISCONNECTED:
      connectedClients--;
      Serial.printf("Client [%u] disconnected. Total clients: %d\n", num, connectedClients);
      break;
    case WStype_TEXT:
      handleWebSocketMessage(num, payload, length);
      break;
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200);

  // Wi-Fi setup
  WiFi.softAP(ssid, password);
  Serial.println("Web server started at: http://" + WiFi.softAPIP().toString());

  // HTTP server setup
  server.on("/", handleRoot);
  server.begin();

  // WebSocket server setup
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  Serial.println("WebSocket server started.");
}

void loop() {
  server.handleClient();
  webSocket.loop();
}
