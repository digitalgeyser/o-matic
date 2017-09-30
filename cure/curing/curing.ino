// Copyright (c) Digital Geyser, 2017

#include <DGMenu.h>

#include <SimpleDHT.h>
#include <Keypad.h>

#define LOG 1
#define REPORT 1

// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

SimpleDHT11 dht11;
DGMenu *dg;

#define pinRED 2
#define pinGREEN 3
#define pinDHT11 4
#define pinRelayFridge 5
#define pinBLUE 6
#define pinRelayDiffuser 13
#define pinWaterLevel A0

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
int waterLevel = -1;

#define PROGRESS_COUNT 2
int progressTick = 0;
const char* progress = "-+";

int fridgeState = 0;
int diffuserState = 1;


void setup() {

#if defined(LOG) || defined(REPORT)
  Serial.begin(9600);
  Serial.println("Startup.");
#endif

  pinMode(pinRelayFridge, OUTPUT);
  pinMode(pinRelayDiffuser, OUTPUT);

  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);

  pinMode(pinWaterLevel, INPUT);
  
  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);

  digitalWrite(pinRelayFridge,fridgeState?LOW:HIGH);
  digitalWrite(pinRelayDiffuser, diffuserState?LOW:HIGH);
  
  dg = new DGMenu(7,8,9,10,11,12, 
                  "Time: DDDd HH:MM", 
                  "T:XXXF RH:XXX%  ");
  dg->refresh();
  cnt = 0;
}

char lastKeyDetected = 0;
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
      lastKeyDetected = key;
    }
  }
}

// report: time, temperature, humidity, fridge status, vaporizer status, water level
void printReport() {
  Serial.print(seconds);
  Serial.print(", ");
  Serial.print(temperature);
  Serial.print(", ");
  Serial.print(humidity);
  Serial.print(", ");
  Serial.print(fridgeState);
  Serial.print(", ");
  Serial.print(diffuserState);
  Serial.print(", ");
  Serial.print(waterLevel);
  Serial.print(", ");
  Serial.print(lastKeyDetected);
  Serial.println("");
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

  waterLevel = analogRead(pinWaterLevel);
    
  if ( !success ) {
#ifdef LOG
    Serial.println("SENSOR ERROR!");
    sensorError = true;
    temperature = -1;
    humidity = -1;
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
    fridgeState = 1;
  } else {
    fridgeState = 0;
  }
  digitalWrite(pinRelayFridge, fridgeState?LOW:HIGH );
  digitalWrite(pinRelayDiffuser, fridgeState?LOW:HIGH );
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

void refreshScreen() {
#ifdef LOG
  Serial.println("Refresh screen");
#endif
   dg->show(14, 0, (seconds/60)%60,     2);
   dg->show(11, 0, (seconds/3600)%24,   2);
   dg->show( 5, 0, (seconds/(3600*24)), 4);
   dg->show( 2, 1, temperature,         3);
   dg->show(10, 1, humidity,            3);
   dg->show(15, 1, progress[progressTick]);

   progressTick = (progressTick+1)%PROGRESS_COUNT;
   dg->refresh();
}

void loop() {

  if ( seconds - lastSeconds >= 2 ) {
   sensorTick();
   refreshLed();
   refreshScreen();  
#ifdef REPORT
   printReport();
#endif
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
