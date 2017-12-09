#include <SPI.h>

#include <SD.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}
void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  ledOn();
  delay(3000);
  ledOff();
  delay(2000);  
}
