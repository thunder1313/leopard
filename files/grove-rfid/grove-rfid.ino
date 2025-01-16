#include <SoftwareSerial.h>
#include <SeeedRFID.h>

#define RFID_RX_PIN 2
#define RFID_TX_PIN 3

SoftwareSerial SoftSerial(RFID_RX_PIN, RFID_TX_PIN);
SeeedRFID RFID(RFID_RX_PIN, RFID_TX_PIN);
RFIDdata tag;

void setup() {
  Serial.begin(115200);      // For debugging on Serial Monitor
  SoftSerial.begin(9600);    // RFID reader's default baud rate is 9600
  Serial.println("\nRFID Reader Test\n");
}

// void loop() {
//   if (RFID.isAvailable()) {
//     tag = RFID.data();
//     Serial.print("Card Number: ");
//     Serial.println(RFID.cardNumber());

//     Serial.print("Raw Data: ");
//     for (int i = 0; i < tag.dataLen; i++) {
//       Serial.print(tag.raw[i], HEX);
//       Serial.print(" ");
//     }
//     Serial.println();
//   }
// }

void loop() {
  // Sprawdź, czy jest nowa karta
  if (RFID.isAvailable()) {
    tag = RFID.data();  // Odczytaj dane karty
    
    Serial.print("Card UID: ");
    dump_byte_array(tag.raw, tag.dataLen);  // Wyświetl UID
    Serial.println();

    Serial.print("Card Number: ");
    Serial.println(RFID.cardNumber());     // Wyświetl numer karty (sformatowany)

    delay(1000); // Opóźnienie, aby unikać wielokrotnego odczytu
  }
}

// Pomocnicza funkcja do wyświetlania UID karty
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}
