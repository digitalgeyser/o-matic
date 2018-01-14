// (c) Digital Geyser, 2018

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <SimpleDHT.h>
#include <DGUtil.h>

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

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#define POLARITY_RELAY_1 31
#define POLARITY_RELAY_2 33

#define TEMPERATURE_HUMIDITY_SENSOR_PIN 35

#define PUMP_TRANSISTOR_PIN 44

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
SimpleDHT11 dht11;

#define OFF 0
#define COOLING 1
#define HEATING 2
int coolingState = -1; // OFF, COOLING, HEATING

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
  redrawCoolingState(160, 100);
}

#define BOXSIZE 80
#define PENRADIUS 3
int oldcolor, currentcolor;
byte temperature = 0;
byte humidity = 0;
char temperatureMode = 'C';
boolean sensorError = false;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("Paint!"));
  
  tft.reset();
  
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101) {     
    identifier=0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
   
  }

  tft.begin(identifier);
  tft.setRotation(2);

  tft.fillScreen(BLACK);

  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, BLUE);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, YELLOW);
  
  currentcolor = RED;
 
  pinMode(13, OUTPUT);
  pinMode(POLARITY_RELAY_1, OUTPUT);  
  pinMode(POLARITY_RELAY_2, OUTPUT);  
  setCoolingState(OFF);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define V_SEP 20
#define H_SEP 13
#define CHAR_SIZE 2

void drawText(int x, int y, const char *txt, uint16_t color) {
  int i, n = strlen(txt);
  tft.fillRect(x, y, x+n*H_SEP, y+V_SEP, BLACK);
  for ( i = 0; i<n; i++ ) {
    tft.drawChar(x+i*H_SEP, y, txt[i], color, BLACK, CHAR_SIZE);
  }
}

void redrawCoolingState(int x, int y) {
  tft.fillRect(x, y, x+100, y+100, BLACK);
  switch(coolingState) {
    case OFF: drawText(x, y, "OFF ", YELLOW); break;
    case COOLING: drawText(x, y, "COOL", BLUE); break;
    case HEATING: drawText(x, y, "HEAT", RED); break;
  }
}

int lastTemperature = -500;
int lastHumidity = -1;

void redrawTemperatureAndHumidity(int x, int y) {
  if ( temperature != lastTemperature || humidity != lastHumidity ) {
  tft.fillRect(x, y, x+100, y+100, BLACK);
  
  tft.drawChar(x, y, 'T', GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+1*H_SEP, y, ':', GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+2*H_SEP, y, '0' + (temperature/100)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+3*H_SEP, y, '0' + (temperature/10)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+4*H_SEP, y, '0' + (temperature)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+5*H_SEP, y, temperatureMode, GREEN, BLACK, CHAR_SIZE);

  y+=V_SEP;
  tft.drawChar(x, y, 'H', GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+1*H_SEP, y, ':', GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+2*H_SEP, y, '0' + (humidity/100)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+3*H_SEP, y, '0' + (humidity/10)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+4*H_SEP, y, '0' + (humidity)%10, GREEN, BLACK, CHAR_SIZE);
  tft.drawChar(x+5*H_SEP, y, '%', GREEN, BLACK, 2);
  }

  lastTemperature = temperature;
  lastHumidity = humidity;
}

void sensorTick() {
  // DHT11 read
  int retries = 3;
  int success;  
  while(retries>0) {
    if (dht11.read(TEMPERATURE_HUMIDITY_SENSOR_PIN, &temperature, &humidity, NULL)) {
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
    /*
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
    */
    
    if (p.y < (TS_MINY-5)) {
      Serial.println("erase");
      // press the bottom of the screen to erase 
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, BLACK);
    }
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    //p.x = tft.width()-map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     //p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    /*
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
    */
    if (p.y < BOXSIZE) {
       oldcolor = currentcolor;

       if (p.x < BOXSIZE) {
         setCoolingState(HEATING);
       } else if (p.x < BOXSIZE*2) {
         setCoolingState(COOLING);
       } else if (p.x < BOXSIZE*3) {
         setCoolingState(OFF);
       }

    }
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
  }
}

