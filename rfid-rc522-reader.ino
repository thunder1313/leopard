#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

constexpr uint8_t RST_PIN = 0;
constexpr uint8_t SS_PIN = 15;

// Definiujemy pin SS i tworzymy instancję sterownika SPI
MFRC522DriverPinSimple ss_pin(SS_PIN); 
MFRC522DriverSPI driver{ss_pin}; 
MFRC522 mfrc522{driver};  // Tworzymy instancję MFRC522

void setup() {
  Serial.begin(115200);     // Inicjalizacja komunikacji szeregowej
  SPI.begin();              // Inicjalizacja magistrali SPI
  mfrc522.PCD_Init();       // Inicjalizacja modułu MFRC522
  
  // Sprawdzanie, czy czytnik jest wykrywany
  if (!mfrc522.PCD_PerformSelfTest()) {
    Serial.println("Failed to detect MFRC522. Check wiring.");
    while (true);  // Zatrzymanie programu, jeśli moduł nie jest wykryty
  }
  Serial.println("MFRC522 initialized. Ready to scan cards.");
}

void loop() {
  // Sprawdzenie, czy nowa karta jest obecna
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(500);  // Krótkie opóźnienie, aby uniknąć spamowania monitora szeregowego
    return;
  }

  // Odczyt UID karty
  if (mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Card UID: ");
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();

    // Zatrzymanie komunikacji z kartą
    mfrc522.PICC_HaltA();
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
