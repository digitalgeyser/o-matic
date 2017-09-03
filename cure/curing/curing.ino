// Copyright (c) Digital Geyser, 2017
#include <SimpleDHT.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

SimpleDHT11 dht11;
LiquidCrystal lcd(7,8,9,10,11,12);

#define pinRED 2
#define pinGREEN 3
#define pinDHT11 4
#define pinRELAY 5
#define pinBLUE 6

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A15, A14, A13, A12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A11, A10, A9, A8}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad kp = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int led = 0;
int cnt;

void setup() {
  Serial.begin(9600);
  pinMode(pinRELAY, OUTPUT);

  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);

  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);

  digitalWrite(pinRELAY,HIGH);
  lcd.begin(16,2);
  lcd.print("Cure-o-matic2000");
  cnt = 0;
}

void loop() {
  // read with raw sample data.
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};

  // DHT11 read
  if (dht11.read(pinDHT11, &temperature, &humidity, data)) {
    Serial.println("Sensor read error!");
    delay(50);
    return;
  }

  // Twiddle the LED
  Serial.print  ("LED:");
  Serial.print  (led & 0x01);
  Serial.print  ( (led & 0x02) >> 1);
  Serial.println( (led & 0x04) >> 2);
  analogWrite(pinRED,   ( (led & 0x0001) == 0) ? 0 : 255);
  analogWrite(pinGREEN, ( (led & 0x0002) == 0) ? 0 : 255);
  analogWrite(pinBLUE,  ( (led & 0x0004) == 0) ? 0 : 255);
  led++;

  
  Serial.print("Sensor bits: ");
  for (int i = 0; i < 40; i++) {
    Serial.print((int)data[i]);
    if (i > 0 && ((i + 1) % 4) == 0) {
      Serial.print(' ');
    }
  }
  Serial.println("");
  
  Serial.print("Sensor data: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" %");

  
  
  if ( humidity > 50 ) {
    digitalWrite(pinRELAY, LOW);
  } else {
    digitalWrite(pinRELAY, HIGH);
  }

  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.print("C");
  lcd.setCursor(4, 1);
  lcd.print("    ");
  lcd.setCursor(4, 1);  
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(8,1);
  lcd.print(cnt++);

  lcd.setCursor(14,1);
  char key = kp.getKey();
  if(key) {
    lcd.print(key);  
  } else {
    lcd.print(" ");
  }
  
  // DHT11 sampling rate is 1HZ.
  delay(2000);
}
