// (c) Digital Geyser, 2018

#include <TouchScreen.h>
#include <SimpleDHT.h>
#include <DGUtil.h>
#include <DGScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

/************************ MACROS **********************************/

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define V_SEP 20
#define H_SEP 13
#define CHAR_SIZE 2

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define POLARITY_RELAY_1 31
#define POLARITY_RELAY_2 33

#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 35
#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 45

#define PUMP_TRANSISTOR_PIN 44

#define DIFFUSER_RELAY 26
#define UNUSED_RELAY 24

#define OFF 0
#define ON 1
#define COOLING 1
#define HEATING 2

#define MODE_AUTO 1
#define MODE_MANUAL 0

#define BUT_W 60
#define BUT_H 40
#define STATUS_Y ((BUT_H * 2) + 15)
#define ROW3 (STATUS_Y + 4*V_SEP + 10)

#define MAX_W 240
#define MAX_H 320

#define IN 0
#define OUT 1

#define MINPRESSURE 10
#define MAXPRESSURE 1000

/********************************  STATE *******************************/

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

DGScreen s(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
SimpleDHT11 dht11;

// State of stuff: -1 is initial state.
int coolingState = -1; // OFF, COOLING, HEATING
int pumpState = -1; // ON, OFF
int diffuserState = -1; // ON, OFF

int oldcolor, currentcolor;
byte temperature[] = { 0, 0 };
byte humidity[] = { 0, 0 };

int lastTemperature[] = { -500, -500 };
int lastHumidity[] = { -1, -1};

int desiredHumidity = 20;
int desiredTemperature = 15;

int count = 0;
int mode = -1;

int onIndicator = 0;
int onCounter  = 0;


/************************************** SETUP **********************************/
void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));

  s.setup(GREEN, BLACK, 2);

  Serial.println(F("Screen setup, filling it BLACK."));
  s.clearScreen();

  s.addButton(0, 0, BUT_W, BUT_H, RED, boxHeat, false);
  s.addButton(BUT_W, 0, BUT_W, BUT_H, BLUE, boxCool, false);
  s.addButton(BUT_W*2, 0, BUT_W, BUT_H, YELLOW, boxOff, false);

  currentcolor = RED;

  s.addButton(0, BUT_H+5, BUT_W, BUT_H, CYAN, pumpOn, false);
  s.addButton(BUT_W, BUT_H+5, BUT_W, BUT_H, CYAN, pumpOff, true);
  s.addButton(BUT_W*2, BUT_H+5, BUT_W, BUT_H, GREEN, diffuserOn, false);
  s.addButton(BUT_W*3, BUT_H+5, BUT_W, BUT_H, GREEN, diffuserOff, true);

  s.addButton(0, ROW3, BUT_W, BUT_H, BLUE, tempMinus, true);
  s.addButton(MAX_W - BUT_W, ROW3, BUT_W, BUT_H, RED, tempPlus, true);
  s.addButton(0, ROW3 + BUT_H + 5, BUT_W, BUT_H, YELLOW, humidityMinus, true);
  s.addButton(MAX_W - BUT_W, ROW3 + BUT_H + 5, BUT_W, BUT_H, GREEN, humidityPlus, true);

  pinMode(13, OUTPUT);
  pinMode(POLARITY_RELAY_1, OUTPUT);
  pinMode(POLARITY_RELAY_2, OUTPUT);
  pinMode(DIFFUSER_RELAY, OUTPUT);
  pinMode(UNUSED_RELAY, OUTPUT);
  pinMode(PUMP_TRANSISTOR_PIN, OUTPUT);
  setMode(MODE_MANUAL);
  setCoolingState(OFF);
  setPump(OFF);
  setDiffuser(OFF);
  digitalWrite(UNUSED_RELAY, HIGH);
  redrawDesiredValues();
}

/******************************* LOOP ******************************************/
void loop() {
  if ( count%5000 == 0 )
    sensorTick();

  if ( count%3000 == 0 && mode == MODE_AUTO )
    autoTick();

  if ( count%50 == 0 ) {
    char ch[11];
    ch[10] = '\0';

    int i;
    for ( i=0; i<10; i++ ) {
      ch[i] = ' ';
    }
    ch[onIndicator] = '.';
    s.drawText(0, MAX_H - V_SEP, ch, YELLOW);
    onIndicator = ( onIndicator + 1 ) % 10;
    s.appendInt(onCounter, 5);
    if ( onIndicator == 0 )
      onCounter++;
  }

  count++;

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, s.width(), 0);
    p.y = (s.height()-map(p.y, TS_MINY, TS_MAXY, s.height(), 0));

    if ( s.processTouch(p.x, p.y) ) {
      Serial.println("Process touch!");
    } else {
      Serial.println("Empty touch!");
    }
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


void pumpOn() { setPump(ON);  setMode(MODE_MANUAL); }
void pumpOff() { setPump(OFF);  setMode(MODE_MANUAL); }
void setPump(int state) {
  if ( pumpState == state ) return;
  switch(state) {
    case ON:
      digitalWrite(PUMP_TRANSISTOR_PIN, HIGH);
      Serial.println(F("Pump on."));
      break;
    case OFF:
      digitalWrite(PUMP_TRANSISTOR_PIN, LOW);
      Serial.println(F("Pump off."));
      break;
  }
  pumpState = state;
  redrawPumpState();
}
void redrawPumpState() {
  int x = 140;
  int y = STATUS_Y + V_SEP;
  switch(pumpState) {
    case OFF: s.drawText(x, y, "AIR OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "AIR ON ", BLUE); break;
  }
}


void boxHeat() { setCoolingState(HEATING);  setMode(MODE_MANUAL); }
void boxCool() { setCoolingState(COOLING);  setMode(MODE_MANUAL); }
void boxOff() { setCoolingState(OFF);  setMode(MODE_MANUAL); }

void setCoolingState(int state) {
  if ( state == coolingState ) return;
  switch(state) {
    case OFF:
      digitalWrite(POLARITY_RELAY_1, HIGH);
      digitalWrite(POLARITY_RELAY_2, HIGH);
      Serial.println(F("Off"));
      break;
    case COOLING:
      digitalWrite(POLARITY_RELAY_1, HIGH);
      digitalWrite(POLARITY_RELAY_2, LOW);
      Serial.println(F("Cool"));
      break;
    case HEATING:
      digitalWrite(POLARITY_RELAY_1, LOW);
      digitalWrite(POLARITY_RELAY_2, HIGH);
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
    case OFF:     s.drawText(x, y, "OFF ", YELLOW); break;
    case COOLING: s.drawText(x, y, "COOL", BLUE);   break;
    case HEATING: s.drawText(x, y, "HEAT", RED);    break;
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
      setPump(OFF);
      setDiffuser(OFF);
    } else if ( humidity[IN] < desiredHumidity - 3 ) {
      setPump(ON);
      setDiffuser(ON);
    }
  }
}

void sensorTick() {
  // DHT11 read
  for ( int i=0; i<2; i++ ) {
    int which = ( i == 0 ? IN : OUT);
    bool success = false;
    int retries = 3;
    while(retries>0) {
      if (dht11.read(which==IN
                      ? TEMPERATURE_HUMIDITY_SENSOR_IN_PIN
                      : TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN,
                    &(temperature[which]),
                    &(humidity[which]),
                    NULL)) {
        retries--;
        success = false;
      } else {
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
      redrawTemperatureAndHumidity(0, STATUS_Y + 2*i*V_SEP, which);
    }
  }


}
