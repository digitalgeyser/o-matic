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

#define DIFFUSER_TRANSISTOR_PIN 39

DGScreen s(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
SimpleDHT11 dht11;

#define OFF 0
#define ON 1
#define COOLING 1
#define HEATING 2
int coolingState = -1; // OFF, COOLING, HEATING
int pumpState = -1;
int diffuserState = -1;

void toggleDiffuser() {
  if ( diffuserState == ON ) {
    setDiffuser(OFF);  
  } else {
    setDiffuser(ON);
  }
}

void pushDiffuserButton() {
  digitalWrite(DIFFUSER_TRANSISTOR_PIN, HIGH);
  delay(100);
  digitalWrite(DIFFUSER_TRANSISTOR_PIN, LOW);  
  delay(100);
}

void setDiffuser(int state) {
  if ( diffuserState == state ) return;
  switch(state) {
    case ON:
      Serial.println(F("Diffuser on."));
      pushDiffuserButton();
      break;
    case OFF:  
      Serial.println(F("Diffuser off."));
      pushDiffuserButton();
      break;
  }
  diffuserState = state;
  redrawDiffuserState(140, 100 + 2*V_SEP);
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

#define ROW2 200
#define BOXSIZE 80

int oldcolor, currentcolor;
byte temperature = 0;
byte humidity = 0;
char temperatureMode = 'C';
boolean sensorError = false;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  s.setup(GREEN, BLACK);
  
  Serial.println(F("Screen setup, filling it BLACK."));
  s.clearScreen();

  s.addButton(0, 0, BOXSIZE, BOXSIZE, RED, boxHeat);
  s.addButton(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLUE, boxCool);
  s.addButton(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, YELLOW, boxOff);
  
  currentcolor = RED;

  s.addButton(0, ROW2, BOXSIZE, BOXSIZE, CYAN, pumpOn);
  s.addButton(BOXSIZE, ROW2, BOXSIZE, BOXSIZE, MAGENTA, pumpOff);
  s.addButton(BOXSIZE*2, ROW2, BOXSIZE, BOXSIZE, GREEN, toggleDiffuser);
  
  pinMode(13, OUTPUT);
  pinMode(POLARITY_RELAY_1, OUTPUT);  
  pinMode(POLARITY_RELAY_2, OUTPUT);  
  pinMode(DIFFUSER_TRANSISTOR_PIN, OUTPUT);  
  pinMode(PUMP_TRANSISTOR_PIN, OUTPUT);  
  setCoolingState(OFF);
  setPump(OFF);
  setDiffuser(OFF);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void redrawDiffuserState(int x, int y) {
  switch(diffuserState) {
    case OFF: s.drawText(x, y, "DIF OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "DIF ON ", BLUE); break;
  }    
}

void redrawPumpState(int x, int y) {
  switch(pumpState) {
    case OFF: s.drawText(x, y, "AIR OFF", YELLOW); break;
    case ON:  s.drawText(x, y, "AIR ON ", BLUE); break;
  }  
}

void redrawCoolingState(int x, int y) {
  switch(coolingState) {
    case OFF: s.drawText(x, y, "OFF ", YELLOW); break;
    case COOLING: s.drawText(x, y, "COOL", BLUE); break;
    case HEATING: s.drawText(x, y, "HEAT", RED); break;
  }
}

int lastTemperature = -500;
int lastHumidity = -1;

void redrawTemperatureAndHumidity(int x, int y) {
  if ( temperature != lastTemperature || humidity != lastHumidity ) {
    s.clearRect(x, y, 100, 2*V_SEP);
  
    s.drawText(x, y, "T:");
    s.appendInt(temperature, 3);
    s.appendChar(temperatureMode);

    y+=V_SEP;
    s.drawText(x, y, "H:");
    s.appendInt(humidity, 3);
    s.appendChar('%');
  }

  lastTemperature = temperature;
  lastHumidity = humidity;
}

void sensorTick() {
  // DHT11 read
  int success;
  int retries = 3;
  while(retries>0) {
    if (dht11.read(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, &temperature, &humidity, NULL)) {
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
    Serial.println("Sensor error!");
    return;
  } else {
    sensorError = false;
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(", humidity: ");
    Serial.println(humidity);
    redrawTemperatureAndHumidity(0, 100);
  }
}

int count = 0;


void loop() {
  if(count%5000 == 0)
    sensorTick();
  count++;
  
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

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

