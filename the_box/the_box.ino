// Libraries used:
//
//  Adafruit unified sensor - by Adafruit
//  DHT Sensor Library - by Adafruit
//  RTClib - by Adafruit
//  LiquidCrystal_I2C - by Frank de Brabander

#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Button.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 22
#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 24

#define IN_SENSOR_TYPE DHT11
#define OUT_SENSOR_TYPE DHT11

#define BUTTON_COUNT 5
Button button_next(22);
Button button_before(23);
Button button_inc(24);
Button button_dec(25);
Button button_sel(26);

Button buttons[5] = { button_next, button_before, button_inc, button_dec, button_sel };

RTC_DS3231 clock;
//DHT insideSensor(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, IN_SENSOR_TYPE);
//DHT outsideSensor(TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, OUT_SENSOR_TYPE);

unsigned long lastLcdRefresh = 0;

void updateLcd(LiquidCrystal_I2C lcd,
               const char *line0,
               const char *line1,
               const char *line2,
               const char *line3) {
  lcd.setCursor ( 0, 0 );
  lcd.print(line0);
  lcd.setCursor ( 0, 1 );
  lcd.print(line1);
  lcd.setCursor ( 0, 2 );
  lcd.print(line2);
  lcd.setCursor ( 0, 3 );
  lcd.print(line3);

}

void setup() {
  int i;

  Serial.begin(115200);
  Serial.println(F("Paint!"));
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  updateLcd(lcd,
            "     The Box!       ",
            "   Digital Geyser   ",
            "                    ",
            "Yummy sausages here!");
  delay(3000);

  clock.begin();

  for ( i = 0; i<BUTTON_COUNT; i++ ) {
    buttons[i].begin();
  }
}


void lcdTick() {
  DateTime now = clock.now();
  char buf2[] = "MM/DD/YYYY  hh:mm:ss";
  lcd.setCursor ( 0, 3 );
  lcd.print(now.toString(buf2));
}

void loop() {
  unsigned long currentTime = millis();

  if ( button_next.pressed() ) Serial.println("next");
  if ( button_before.pressed() ) Serial.println("before");
  if ( button_inc.pressed() ) Serial.println("inc");
  if ( button_dec.pressed() ) Serial.println("dec");
  if ( button_sel.pressed() ) Serial.println("sel");

  if ( currentTime - lastLcdRefresh > 1000 ) {
    lcdTick();
    lastLcdRefresh = currentTime;
  }
}
