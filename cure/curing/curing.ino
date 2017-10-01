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

int lastKeyPressedSeconds = 0;

byte thresholdHumidity = 90;
byte thresholdTemperature = 50;

boolean sensorError = false;
byte temperature = 0;
byte humidity = 0;
int waterLevel = -1;
char lastKeyDetected = 0;
char previousKey = 0;


#define PROGRESS_COUNT 2
int progressTick = 0;
const char* progress = "-+";
const char* line1 = "Time: DDDd HH:MM";
const char* line2 = "T:XXXF RH:XXX%  ";
int fridgeState = 0;
int diffuserState = 1;

boolean menuState = 0;

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
                  line1, 
                  line2);
  dg->refresh();
  cnt = 0;
}

// Returns true if menu state has to change
boolean keyTick() {
  char key = kp.getKey();
  boolean change = false;
  if(key!=previousKey) {
    change = true;
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
      lastKeyPressedSeconds = seconds;

      switch(key) {
      case '*': menuState = 1-menuState; break;
      case '1': thresholdHumidity--; menuState = 1; break;
      case '2': thresholdHumidity++; menuState = 1; break;
      case '4': thresholdTemperature--; menuState = 1; break;
      case '5': thresholdTemperature++; menuState = 1; break;
      default: menuState = 1; break;
      }
    }
  }
  if ( (menuState) && (!change) && (seconds > 20 + lastKeyPressedSeconds) ) {
    menuState = 0;
    change = true;
  }
  return change;
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
    sensorError = true;
    temperature = -1;
    humidity = -1;
    return;
  } else {
    sensorError = false;
  }
  
  if ( temperature > thresholdTemperature ) {
    fridgeState = 1;
  } else {
    fridgeState = 0;
  }
  if ( humidity < thresholdHumidity ) {
    diffuserState = 1;
  } else {
    diffuserState = 0;
  }
  digitalWrite(pinRelayFridge, fridgeState?LOW:HIGH );
  digitalWrite(pinRelayDiffuser, diffuserState?LOW:HIGH );
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
  if ( menuState ) {
    refreshMenu();
  } else {
    refreshDefaultScreen();
  }
} 

void refreshMenu() {
  dg->screen("RH [1/2]:      %",
             " T [4/5]:      F");
  dg->show(12, 0, thresholdHumidity, 3);
  dg->show(12, 1, thresholdTemperature, 3);
  dg->refresh();
}

void refreshDefaultScreen() {
#ifdef LOG
  Serial.println("Refresh screen");
#endif
  dg->screen(line1, line2);
  dg->show(14, 0, (seconds/60)%60,     2);
  dg->show(11, 0, (seconds/3600)%24,   2);
  dg->show( 5, 0, (seconds/(3600*24)), 4);
  if ( sensorError ) {
    dg->show(0, 1, "Sensor error! ");
  } else {
    dg->show( 2, 1, temperature,         3);
    dg->show(10, 1, humidity,            3);
  }
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

  if(keyTick()) {
    refreshScreen();  
  }
  
  
  // DHT11 sampling rate is 1HZ.
  delay(10);
  miliseconds += 10;
  if ( miliseconds >= 1000 ) {
    miliseconds = 0;
    seconds++;
  }
}
