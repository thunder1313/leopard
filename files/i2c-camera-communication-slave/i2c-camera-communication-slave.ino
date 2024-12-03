#include <Wire.h>
#define XIAO_ADDR 0x42
#define SDA_PIN 4
#define SCL_PIN 5
#define LED_PIN 21

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN, XIAO_ADDR);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // digitalWrite(LED_PIN, LOW);
  // delay (1000);
  // digitalWrite(LED_PIN, HIGH);
  // delay (1000);
}

void receiveEvent(int bytes) {
  while (Wire.available()) {
    char command = Wire.read(); // Read command
    if (command == '1') {
      digitalWrite(LED_PIN, HIGH); // Turn LED on
    } else if (command == '0') {
      digitalWrite(LED_PIN, LOW);  // Turn LED off
    }
  }
}
