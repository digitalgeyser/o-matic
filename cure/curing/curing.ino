// Copyright (c) Digital Geyser, 2017

#include <DGMenu.h>
#include <DGUtil.h>
#include <DGKey.h>

#include <SimpleDHT.h>

//#define LOG 1
#define REPORT 1

// for DHT11,
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2

SimpleDHT11 dht11;
DGMenu *dg;
DGKey *dk;

#define pinRED 2
#define pinGREEN 3
#define pinDHT11 4
#define pinBLUE 6
#define pinRelayHumidityUp A0
#define pinRelayHumidityDown A1

#define pinRelayPolarity0 A3
#define pinRelayPolarity1 A4

#define PROGRESS_COUNT 2


char temperatureMode = 'C';

int cnt;

int miliseconds = 0;
long seconds = 0;
long lastSeconds = 0;
long lastReinit = 0;

int lastKeyPressedSeconds = 0;

byte thresholdHumidity = 90;
byte thresholdTemperature = 10;

boolean sensorError = false;
byte temperature = 0;
byte humidity = 0;
int humidityState = 0;
int waterLevel = -1;
char lastKeyDetected = 0;
char previousKey = 0;

int progressTick = 0;
const char* progress = ": ";
const char* line1 = "Time: DDDd HH:MM";
const char* line2 = "T:XXXC RH:XXX%  ";
int temperatureGradient = 0;

boolean menuState = 0;

void setup() {

#if defined(LOG) || defined(REPORT)
  Serial.begin(9600);
  Serial.println("Startup.");
#endif

  pinMode(pinRelayHumidityUp, OUTPUT);
  pinMode(pinRelayHumidityDown, OUTPUT);
  pinMode(pinRelayPolarity0, OUTPUT);
  pinMode(pinRelayPolarity1, OUTPUT);

  pinMode(pinRED, OUTPUT);
  pinMode(pinGREEN, OUTPUT);
  pinMode(pinBLUE, OUTPUT);

  digitalWrite(pinRED, HIGH);
  digitalWrite(pinGREEN, LOW);
  digitalWrite(pinBLUE, LOW);

  digitalWrite(pinRelayHumidityUp, LOW);
  digitalWrite(pinRelayHumidityDown, LOW);
  
  dk = new DGKey();
  dg = new DGMenu(7,8,9,10,11,12, line1, line2);
  dg->refresh();
  cnt = 0;
  setFridge();
}

void setFridge() {
  switch(temperatureGradient) {
    case -1:
      digitalWrite(pinRelayPolarity0,HIGH);
      digitalWrite(pinRelayPolarity1,LOW);
      break;
    case 0:
      digitalWrite(pinRelayPolarity0,LOW);
      digitalWrite(pinRelayPolarity1,LOW);
      break;
    case 1:  
      digitalWrite(pinRelayPolarity0,LOW);
      digitalWrite(pinRelayPolarity1,HIGH);
      break;
  }
}

// Returns true if menu state has to change
boolean keyTick() {
  char key = dk->getKey();
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
      case 'C': toggleTemperatureMode(); menuState = 1; break;
      case 'D': dg->reinit(); break;
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

void toggleTemperatureMode() {
  switch(temperatureMode) {
    case 'F':
      temperatureMode = 'C';
      thresholdTemperature = DGUtil::temperatureFtoC(thresholdTemperature);
      temperature = DGUtil::temperatureFtoC(thresholdTemperature);
      break;
    case 'C':
      temperatureMode = 'F';
      thresholdTemperature = DGUtil::temperatureCtoF(thresholdTemperature);
      temperature = DGUtil::temperatureCtoF(thresholdTemperature);
      break;
  }
}

// report: time, temperature, humidity, fridge status, vaporizer status, water level
void printReport() {
  Serial.print(sensorError);
  Serial.print(", ");
  Serial.print(seconds);
  Serial.print(", ");
  Serial.print(temperature);
  Serial.print(", ");
  Serial.print(humidity);
  Serial.print(", ");
  Serial.print(temperatureGradient);
  Serial.print(", ");
  Serial.print(humidityState);
  Serial.print(", ");
  Serial.print(lastKeyDetected);
  Serial.println("");
  Serial.flush();
  
}

void sensorTick() {
  // DHT11 read
  int retries = 3;
  int success;  
  while(retries>0) {
    if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
     retries--;
     success = 0;
     if ( temperatureMode == 'F' ) {
        temperature = DGUtil::temperatureCtoF(temperature);
     }
   } else {
     success = 1;
     break;
   }
  }

  if ( !success ) {
    sensorError = true;
    temperature = -1;
    humidity = -1;
    return;
  } else {
    sensorError = false;
  }

  if ( temperature < thresholdTemperature - 2 ) {
    temperatureGradient = 1;
  } else if ( temperature > thresholdTemperature + 2 ) {
    temperatureGradient = -1;
  } else {
    temperatureGradient = 0;
  }

  if ( humidity > thresholdHumidity + 2 ) {
    humidityState = 1;
  } else if ( humidity < thresholdHumidity - 2 ) {
    humidityState = -1;
  } else {
    humidityState = 0;
  }
  
  setFridge();  
  digitalWrite(pinRelayHumidityDown, humidityState>0?LOW:HIGH );
  digitalWrite(pinRelayHumidityUp, humidityState<0?LOW:HIGH );
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

  if ( seconds - lastReinit > 60 ) {
    dg->reinit();
    lastReinit = seconds;
  }
  if ( menuState ) {
    refreshMenu();
  } else {
    refreshDefaultScreen();
  }
}

void refreshMenu() {
  dg->screen("RH [1/2]:      %",
             " T [4/5]:      C");
  dg->show(15, 1, temperatureMode);
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
  dg->show( 5, 0, (seconds/3600)/24,   4);
  dg->show( 5, 1, temperatureMode);
  if ( sensorError ) {
    dg->show(0, 1, "Sensor error! ");
  } else {
    dg->show( 2, 1, temperature,         3);
    dg->show(10, 1, humidity,            3);
  }

  // Progress tick at the space between hours/minutes.
  dg->show(13, 0, progress[progressTick]);
  progressTick = (progressTick+1)%PROGRESS_COUNT;

  // + or - at 15 for fridge state
  dg->show(15, 1, (temperatureGradient==1?'+':( temperatureGradient == -1 ? '-' : ' ') ) );
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
