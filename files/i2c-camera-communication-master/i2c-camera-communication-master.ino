#include <Wire.h>

void setup() {
  Wire.begin(D1, D2); // D1 = SCL, D2 = SDA (GPIO5, GPIO4)
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(0x42); // Example I2C address
  Wire.write('1');
  Wire.endTransmission();
  delay(1000);

  Wire.beginTransmission(0x42); // Example I2C address
  Wire.write('0');
  Wire.endTransmission();
  delay(1000);
}
