// (c) Digital Geyser, 2018

#include <TouchScreen.h>
#include <SimpleDHT.h>
#include <DGUtil.h>
#include <DGScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif


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

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4



#define POLARITY_RELAY_1 31
#define POLARITY_RELAY_2 33

#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 35
#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 45

#define PUMP_TRANSISTOR_PIN 44

#define DIFFUSER_RELAY 26
#define UNUSED_RELAY 24

DGScreen s(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
SimpleDHT11 dht11;

#define OFF 0
#define ON 1
#define COOLING 1
#define HEATING 2

// State of stuff: -1 is initial state.
int coolingState = -1; // OFF, COOLING, HEATING
int pumpState = -1; // ON, OFF
int diffuserState = -1; // ON, OFF

#define ROW2 200
#define BOXSIZE 60

int oldcolor, currentcolor;
byte temperature[] = { 0, 0 };
byte humidity[] = { 0, 0 };
char temperatureMode = 'C';

#define IN 0
#define OUT 1
int lastTemperature[] = { -500, -500 };
int lastHumidity[] = { -1, -1};


#define MINPRESSURE 10
#define MAXPRESSURE 1000

int count = 0;


/************************************** SETUP **********************************/
void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  s.setup(GREEN, BLACK, 2);
  
  Serial.println(F("Screen setup, filling it BLACK."));
  s.clearScreen();

  s.addButton(0, 0, BOXSIZE, BOXSIZE, RED, boxHeat);
  s.addButton(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLUE, boxCool);
  s.addButton(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, YELLOW, boxOff);
  
  currentcolor = RED;

  s.addButton(0, ROW2, BOXSIZE, BOXSIZE, CYAN, pumpOn);
  s.addButton(BOXSIZE, ROW2, BOXSIZE, BOXSIZE, MAGENTA, pumpOff);
  s.addButton(BOXSIZE*2, ROW2, BOXSIZE, BOXSIZE, GREEN, diffuserOn);
  s.addButton(BOXSIZE*3, ROW2, BOXSIZE, BOXSIZE, WHITE, diffuserOff);
  
  pinMode(13, OUTPUT);
  pinMode(POLARITY_RELAY_1, OUTPUT);  
  pinMode(POLARITY_RELAY_2, OUTPUT);  
  pinMode(DIFFUSER_RELAY, OUTPUT);  
  pinMode(UNUSED_RELAY, OUTPUT);  
  pinMode(PUMP_TRANSISTOR_PIN, OUTPUT);  
  setCoolingState(OFF);
  setPump(OFF);
  setDiffuser(OFF);
  digitalWrite(UNUSED_RELAY, HIGH);
}

/******************************* LOOP ******************************************/
void loop() {
  if(count%5000 == 0)
    sensorTick();
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

void diffuserOn() { setDiffuser(ON); }
void diffuserOff() { setDiffuser(OFF); }
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
  redrawDiffuserState(140, 100 + 2*V_SEP);
}
void redrawDiffuserState(int x, int y) {
  switch(diffuserState) {
    case OFF: s.drawText(x, y, "DIF OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "DIF ON ", BLUE); break;
  }    
}


void pumpOn() { setPump(ON); }
void pumpOff() { setPump(OFF); }
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
  redrawPumpState(140, 100 + V_SEP);
}
void redrawPumpState(int x, int y) {
  switch(pumpState) {
    case OFF: s.drawText(x, y, "AIR OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "AIR ON ", BLUE); break;
  }  
}


void boxHeat() { setCoolingState(HEATING); }
void boxCool() { setCoolingState(COOLING); }
void boxOff() { setCoolingState(OFF); }

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
  redrawCoolingState(140, 100);
}

void redrawCoolingState(int x, int y) {
  switch(coolingState) {
    case OFF:     s.drawText(x, y, "OFF ", YELLOW); break;
    case COOLING: s.drawText(x, y, "COOL", BLUE);   break;
    case HEATING: s.drawText(x, y, "HEAT", RED);    break;
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
    s.appendChar(temperatureMode);

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

void sensorTick() {
  // DHT11 read
  for ( int i=0; i<2; i++ ) {
    int which = ( i == 0 ? IN : OUT);
    bool success = false;
    int retries = 3;
    while(retries>0) {
      if (dht11.read(which==IN?TEMPERATURE_HUMIDITY_SENSOR_IN_PIN:TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, &(temperature[which]), &(humidity[which]), NULL)) {
        retries--;
        success = false;
      } else {
        success = true;
        if ( temperatureMode == 'F' ) {
          temperature[which] = DGUtil::temperatureCtoF(temperature[which]);
        }
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
      redrawTemperatureAndHumidity(0, 100 + 2*i*V_SEP, which);
    }
  }
 
  
}


