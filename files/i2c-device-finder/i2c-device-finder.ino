#include <Wire.h>

void setup() {
  Wire.begin(D1, D2);
  Serial.begin(9600);
}

void loop() {
  byte error;
  int nDevices = 0;

  for (int address = 0x00; address < 0x80; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0x00) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0) Serial.println("No I2C devices found");
  delay(5000);
}

