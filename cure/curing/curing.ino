// (c) Digital Geyser, 2018

#include <DHT.h>
#include <DGUtil.h>
#include <DGScreen.h>

#include "box_programs.h"

#include <DS3231.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

/************************ MACROS **********************************/

#define V_SEP 20
#define H_SEP 13
#define CHAR_SIZE 2

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define PELTIER_RELAY1 35
#define PELTIER_RELAY2 33

#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 23
#define IN_SENSOR_TYPE DHT11

#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 45
#define OUT_SENSOR_TYPE DHT11

#define HUMID_PUMP_TRANSISTOR_PIN 44
#define DRY_PUMP_TRANSISTOR_PIN 42

#define FAN_PWM 27

#define DIFFUSER_RELAY 26
#define UNUSED_RELAY 24

#define OFF 0
#define ON 1
#define COOLING 1
#define HEATING 2

#define HUMIDIFYING 1
#define DRYING 2

#define MODE_AUTO 1
#define MODE_MANUAL 0

#define BUT_W 60
#define BUT_H 40
#define STATUS_Y ((BUT_H * 2) + 15)
#define ROW3 (STATUS_Y + 4*V_SEP + 10)


#define IN 0
#define OUT 1

/********************************  STATE *******************************/

DGScreen s(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
DHT insideSensor(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, IN_SENSOR_TYPE);
DHT outsideSensor(TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, OUT_SENSOR_TYPE);

// State of stuff: -1 is initial state.
int coolingState = -1; // OFF, COOLING, HEATING
int humidifyingState = -1; // OFF, DRYING, HUMIDIFYING
int diffuserState = -1; // ON, OFF

byte temperature[] = { 0, 0 };
byte humidity[] = { 0, 0 };

int lastTemperature[] = { -500, -500 };
int lastHumidity[] = { -1, -1};

int desiredHumidity = 70;
int desiredTemperature = 15;

int mode = -1;

DS3231 clock;


/************************************** SETUP **********************************/
void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));

  s.setup(GREEN, BLACK, 0);

  Serial.println(F("Screen setup, filling it BLACK."));
  s.clearScreen();

  s.addButton(0,       0, BUT_W, BUT_H, "Cool", BLUE, BLACK, boxCool, false);
  s.addButton(BUT_W,   0, BUT_W, BUT_H, "Off", YELLOW, YELLOW, boxOff, true);
  s.addButton(BUT_W*2, 0, BUT_W, BUT_H, "Heat", RED, BLACK, boxHeat, false);
  s.addButton(BUT_W*3, 0, BUT_W, BUT_H, "Dif On", CYAN, BLACK, diffuserOn, false);

  s.addButton(0,       BUT_H+5, BUT_W, BUT_H, "Humid", BLUE,   BLACK, pumpHumid, false);
  s.addButton(BUT_W,   BUT_H+5, BUT_W, BUT_H, "Off", YELLOW, YELLOW, pumpOff,   true);
  s.addButton(BUT_W*2, BUT_H+5, BUT_W, BUT_H, "Dry", RED,    BLACK, pumpDry,   false);
  s.addButton(BUT_W*3, BUT_H+5, BUT_W, BUT_H, "Df Off", CYAN, CYAN, diffuserOff, true);

  s.addButton(0,             ROW3,             BUT_W, BUT_H, "Cool",  BLUE,   BLUE,   tempMinus,     true);
  s.addButton(s.width() - BUT_W, ROW3,             BUT_W, BUT_H, "Heat",  RED,    RED,    tempPlus,      true);
  s.addButton(0,             ROW3 + BUT_H + 5, BUT_W, BUT_H, "Dry", YELLOW, YELLOW, humidityMinus, true);
  s.addButton(s.width() - BUT_W, ROW3 + BUT_H + 5, BUT_W, BUT_H, "Humid", GREEN,  GREEN,  humidityPlus,  true);

  pinMode(13, OUTPUT);
  pinMode(PELTIER_RELAY1, OUTPUT);
  pinMode(PELTIER_RELAY2, OUTPUT);
  pinMode(DIFFUSER_RELAY, OUTPUT);
  pinMode(UNUSED_RELAY, OUTPUT);
  pinMode(HUMID_PUMP_TRANSISTOR_PIN, OUTPUT);
  pinMode(DRY_PUMP_TRANSISTOR_PIN, OUTPUT);
  pinMode(FAN_PWM, OUTPUT);
  setMode(MODE_MANUAL);
  setCoolingState(OFF);
  setPump(OFF);
  setDiffuser(OFF);
  digitalWrite(UNUSED_RELAY, HIGH);
  redrawDesiredValues();

  Serial.println(F("Starting RTC."));
  clock.begin();
  Serial.println(F("Setup done."));
}


/******************************* LOOP ******************************************/
void loop() {
  static int count = 0;

  if ( count%100 == 0 ) {
    realTimeTick();
  }

  count++;

  s.touchScreen();
}

// This method takes the unmeasured occasional tick and converts it to more accurate minute and seconds ticks
void realTimeTick() {
  static int lastSec = -1;
  static int lastMin = -1;
  RTCDateTime dt = clock.getDateTime();
  
  if ( dt.minute != lastMin ) {
    minuteTick(dt.minute);
    lastMin = dt.minute;
    s.drawText(0, s.height() - V_SEP, clock.dateFormat("m/d/Y H:i", dt), GREEN);
    Serial.println("Minute.");
  }

  if ( dt.second != lastSec ) {
    secondTick(dt.second);
    lastSec = dt.second;
    Serial.println("Second.");
  }
}

void minuteTick(int minute) {
  
}

void secondTick(int second) {
  if ( second % 10 == 0 ) {
    // Every 10 seconds, deal with sensors
    sensorTick();
  }

  if ( second % 5 == 0 && mode == MODE_AUTO ) {
    // Every 5 seconds readjust the automatics
    autoTick();
  }
}

/*************************************************** Functions ****************************/

void diffuserOn() { setDiffuser(ON); setMode(MODE_MANUAL); }
void diffuserOff() { setDiffuser(OFF); setMode(MODE_MANUAL); }
void setDiffuser(int state) {
  if ( diffuserState == state ) return;
  switch(state) {
    case ON:
      Serial.println(F("Diffuser on."));
      digitalWrite(DIFFUSER_RELAY, LOW);
      break;
    case OFF:
      Serial.println(F("Diffuser off."));
      digitalWrite(DIFFUSER_RELAY, HIGH);
      break;
  }
  diffuserState = state;
  redrawDiffuserState();
}
void redrawDiffuserState() {
  int x = 140;
  int y = STATUS_Y + 2*V_SEP;
  switch(diffuserState) {
    case OFF: s.drawText(x, y, "DIF OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "DIF ON ", BLUE); break;
  }
}


void pumpHumid() { setPump(HUMIDIFYING);  setMode(MODE_MANUAL); }
void pumpDry() { setPump(DRYING);  setMode(MODE_MANUAL); }
void pumpOff() { setPump(OFF);  setMode(MODE_MANUAL); }
void setPump(int state) {
  if ( humidifyingState == state ) return;
  switch(state) {
    case HUMIDIFYING:
      digitalWrite(DRY_PUMP_TRANSISTOR_PIN, LOW);
      digitalWrite(HUMID_PUMP_TRANSISTOR_PIN, HIGH);
      Serial.println(F("Pump humidifying."));
      break;
    case DRYING:
      digitalWrite(HUMID_PUMP_TRANSISTOR_PIN, LOW);
      digitalWrite(DRY_PUMP_TRANSISTOR_PIN, HIGH);
      Serial.println(F("Pump drying."));
      break;
    case OFF:
      digitalWrite(HUMID_PUMP_TRANSISTOR_PIN, LOW);
      digitalWrite(DRY_PUMP_TRANSISTOR_PIN, LOW);
      Serial.println(F("Pump off."));
      break;
  }
  humidifyingState = state;
  redrawPumpState();
}

void redrawPumpState() {
  int x = 140;
  int y = STATUS_Y + V_SEP;
  switch(humidifyingState) {
    case OFF:          s.drawText(x, y, "PMP OFF", YELLOW); break;
    case HUMIDIFYING:  s.drawText(x, y, "HUMID  ", BLUE); break;
    case DRYING:       s.drawText(x, y, "DRY    ", RED); break;
  }
}


void boxHeat() { setCoolingState(HEATING);  setMode(MODE_MANUAL); }
void boxCool() { setCoolingState(COOLING);  setMode(MODE_MANUAL); }
void boxOff() { setCoolingState(OFF);  setMode(MODE_MANUAL); }

void setCoolingState(int state) {
  if ( state == coolingState ) return;
  switch(state) {
    case OFF:
      digitalWrite(PELTIER_RELAY1, LOW);
      digitalWrite(PELTIER_RELAY2, LOW);
      Serial.println(F("Off"));
      break;
    case COOLING:
      digitalWrite(PELTIER_RELAY1, LOW);
      digitalWrite(PELTIER_RELAY2, LOW);
      digitalWrite(PELTIER_RELAY2, HIGH);
      Serial.println(F("Cool"));
      break;
    case HEATING:
      digitalWrite(PELTIER_RELAY1, LOW);
      digitalWrite(PELTIER_RELAY2, LOW);
      digitalWrite(PELTIER_RELAY1, HIGH);
      Serial.println(F("Heat"));
      break;
  }
  coolingState = state;
  redrawCoolingState();
}


void redrawCoolingState() {
  int x = 140;
  int y = STATUS_Y;
  switch(coolingState) {
    case OFF:     s.drawText(x, y, "HC OFF ", YELLOW); break;
    case COOLING: s.drawText(x, y, "COOL   ", BLUE);   break;
    case HEATING: s.drawText(x, y, "HEAT   ", RED);    break;
  }
}

void setMode(int newMode) {
  if ( newMode == mode ) return;
  mode = newMode;
  redrawMode();
}

void redrawMode() {
  int x = 140;
  int y = STATUS_Y + 3*V_SEP;
  switch(mode) {
    case MODE_MANUAL: s.drawText(x, y, "MANUAL ", RED); break;
    case MODE_AUTO:   s.drawText(x, y, "AUTO   ", GREEN); break;
  }
}

void redrawTemperatureAndHumidity(int x, int y, int which) {
  if ( temperature[which] != lastTemperature[which] || humidity[which] != lastHumidity[which] ) {
    s.clearRect(x, y, 120, 2*V_SEP);

    if (which==IN) {
      s.drawText(x, y, "T_i:");
    } else {
      s.drawText(x, y, "T_o:");
    }
    s.appendInt(temperature[which], 3);
    s.appendChar('C');

    y+=V_SEP;
    if (which==IN) {
      s.drawText(x, y, "H_i:");
    } else {
      s.drawText(x, y, "H_o:");
    }
    s.appendInt(humidity[which], 3);
    s.appendChar('%');
  }

  lastTemperature[which] = temperature[which];
  lastHumidity[which] = humidity[which];
}

void tempMinus() { tempDiff(-1); }
void tempPlus() { tempDiff(1); }
void tempDiff(int diff) {
  desiredTemperature += diff;
  redrawDesiredValues();
  setMode(MODE_AUTO);
}

void humidityMinus() { humidityDiff(-1); }
void humidityPlus() { humidityDiff(1); }
void humidityDiff(int diff) {
  desiredHumidity += diff;
  redrawDesiredValues();
  setMode(MODE_AUTO);
}
void redrawDesiredValues() {
  s.drawText(BUT_W+20, ROW3 + 5, "T:");
  s.appendInt(desiredTemperature, 3);
  s.appendChar('C');

  s.drawText(BUT_W+20, ROW3 + 5 + BUT_W, "H:");
  s.appendInt(desiredHumidity, 3);
  s.appendChar('%');
}

// This function is responsible for the automatic adjustments based on
// sensor data.
void autoTick() {
  if ( temperature[IN] != -1 ) {
    // We have real sensor data, let's act on it
    switch(coolingState) {
      case COOLING:
        if ( temperature[IN] <= desiredTemperature )
          setCoolingState(OFF);
        break;
      case HEATING:
        if ( temperature[IN] >= desiredTemperature )
          setCoolingState(OFF);
        break;
      case OFF:
        if ( temperature[IN] >= desiredTemperature + 3 )
          setCoolingState(COOLING);
        else if ( temperature[IN] <= desiredTemperature - 3)
          setCoolingState(HEATING);
        break;
    }
  }

  if ( humidity[IN] != -1 ) {
    // We have data
    if ( humidity[IN] > desiredHumidity + 3 ) {
      setPump(DRYING);
      setDiffuser(OFF);
    } else if ( humidity[IN] < desiredHumidity - 3 ) {
      setPump(HUMIDIFYING);
      setDiffuser(ON);
    } else {
      setPump(OFF);
      setDiffuser(OFF);
    }
  }
}


void sensorTick() {
  // DHT11 read
  for ( int i=0; i<2; i++ ) {
    int which = ( i == 0 ? IN : OUT);
    
    bool success = false;
    int retries = 3;
    DHT *dhtToUse = (which == IN ? &insideSensor : &outsideSensor );
    
    // Temporary: we don't have out sensor yet
    if ( which == OUT ) continue;
    
    while(retries>0) {
      float t = dhtToUse->readTemperature();
      float h = dhtToUse->readHumidity();
        
      if ( isnan(t) || isnan(h) ) {
        retries--;
        success = false;
      } else {
        temperature[which] = (int)t;
        humidity[which] = (int)h;
        success = true;
        break;
      }
    }


    if ( !success ) {
      temperature[which] = -1;
      humidity[which] = -1;
      Serial.print("Sensor error:");
      Serial.println(which);
    } else {
      Serial.print("Sensor data:");
      Serial.print(which);
      Serial.print(", t=");
      Serial.print(temperature[which]);
      Serial.print(", h=");
      Serial.print(humidity[which]);
      Serial.println("");
      redrawTemperatureAndHumidity(0, STATUS_Y + 2*i*V_SEP, which);
    }
  }


}
