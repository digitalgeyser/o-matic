#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DGBme280.h>

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
int BME_SELECT = A6;

int RED = A0;
int GREEN = A1;
int BLUE = A2;

DGBme280 bme(0x76);

#define SEALEVELPRESSURE_HPA (1013.25)

void setup() {

  pinMode(BME_SELECT, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(BME_SELECT, HIGH);

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

  Serial.print("BME 280 ...");
  if ( !bme.begin() ) {
    Serial.println(" failed.");
  } else {
    Serial.println(" OK!");
  }

}

void ledTick() {
  static int tick = 0;
  digitalWrite(RED, tick%3 == 0);
  digitalWrite(GREEN, tick%3 == 1);
  digitalWrite(BLUE, tick%3 == 2);
  tick = (tick+1)%3;
}

void printValues() {
  bme.readSensor();
  Serial.print(bme.getPressure_MB()); Serial.print(" mb\t"); // Pressure in millibars
  Serial.print(bme.getHumidity()); Serial.print(" %\t\t");
  Serial.print(bme.getTemperature_C()); Serial.print(" *C\t");
  Serial.print(bme.getTemperature_F()); Serial.println(" *F");
}

void sensorTick() {
  static int tick = 0;
  if ( tick%10 == 0 ) {
    printValues();
  }
  tick = (tick+1)%10;
}

void loop() {
  ledTick();
  sensorTick();
  delay(200);
}
