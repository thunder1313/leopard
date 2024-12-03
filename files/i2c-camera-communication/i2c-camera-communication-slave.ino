#include <Wire.h>

#define XIAO_ADDR 1

void setup() {
  Wire.begin();
  Serial.begin(115200);

}

void loop() {
  Wire.beginTransmission(XIAO_ADDR);

}
