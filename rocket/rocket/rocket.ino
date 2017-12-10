#include <SPI.h>
#include <SD.h>

// Pin layout:
// RED led A0
// GREEN led A1
// BLUE led A2
// CS - pin 10
// MISO - pin 12
// MOSI - pin 11 
// SCK - pin 13
// A6 chipselect for BME280
// A5 SCL for I2C
// A4 SDA for I2C

int chipSelect=10;

int RED = A0;
int GREEN = A1;
int BLUE = A2;

void setup() {

   // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("SD ...");

  if (!SD.begin(chipSelect)) {
    Serial.println(" failed.");
  } else {
    Serial.println(" OK!");
  }
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
}

void ledTick() {
  static int tick = 0;
  digitalWrite(RED, tick%3 == 0);
  digitalWrite(GREEN, tick%3 == 1);
  digitalWrite(BLUE, tick%3 == 2);
  tick = (tick+1)%3;
}

void loop() {
  ledTick();
  delay(200);
}
