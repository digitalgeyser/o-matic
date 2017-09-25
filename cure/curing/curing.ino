#include <DGMenu.h>

// Copyright (c) Digital Geyser, 2017
#include <SimpleDHT.h>
#include <Keypad.h>

#define LOG 1

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

SimpleDHT11 dht11;
DGMenu *dg;

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


int cnt;

int miliseconds = 0;
long seconds = 0;
long lastSeconds = 0;

boolean sensorError = false;
byte temperature = 0;
byte humidity = 0;

void setup() {

#ifdef LOG
  Serial.begin(9600);
#endif

  pinMode(pinRELAY, OUTPUT);

  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);

  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);

  digitalWrite(pinRELAY,HIGH);

  dg = new DGMenu(7,8,9,10,11,12);
  cnt = 0;
}

char previousKey = 0;

void keyTick() {
  char key = kp.getKey();
  if(key!=previousKey) {
    previousKey = key;
    if ( key == 0 ) {
#ifdef LOG
      Serial.println("Key off.");
#endif
    } else {
#ifdef LOG
      Serial.print("Key:"); Serial.println(key);
#endif
    }
  }
}

void sensorTick() {
  // DHT11 read
  int retries = 3;
  int success;
  while(retries>0) {
    if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
     retries--;
     success = 0;
   } else {
     success = 1;
     break;
   }
  }

  if ( !success ) {
#ifdef LOG
  Serial.println("SENSOR ERROR!");
  sensorError = true;
  return;
#endif
  } else {
    sensorError = false;
  }
  
#ifdef LOG  
  Serial.print("Sensor data: ");
  Serial.print((int)temperature); Serial.print(" *C, ");
  Serial.print((int)humidity); Serial.println(" %");
#endif

  if ( humidity > 75 ) {
    digitalWrite(pinRELAY, LOW);
  } else {
    digitalWrite(pinRELAY, HIGH);
  }

}

void refreshLed() {
  if ( sensorError ) {
    analogWrite(pinRED,   255 );
    analogWrite(pinGREEN,   0 );
    analogWrite(pinBLUE,    0 );    
  } else {
    analogWrite(pinRED,     0 );
    analogWrite(pinGREEN, 255 );
    analogWrite(pinBLUE,    0 );
  }
}

void loop() {


  if ( seconds - lastSeconds >= 2 ) {
   sensorTick();
   refreshLed();
   lastSeconds = seconds;
  }
  
  keyTick();  
  
  
  // DHT11 sampling rate is 1HZ.
  delay(10);
  miliseconds += 10;
  if ( miliseconds >= 1000 ) {
    miliseconds = 0;
    seconds++;
  }
}
