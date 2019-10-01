#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 22
#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 24

#define IN_SENSOR_TYPE DHT11
#define OUT_SENSOR_TYPE DHT11

DS3231 clock;
DHT insideSensor(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, IN_SENSOR_TYPE);
DHT outsideSensor(TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, OUT_SENSOR_TYPE);

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

  clock.begin();
}

void setup() {
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  updateLcd(lcd,
            "     The Box!       ",
            "   Digital Geyser   ",
            "                    ",
            "  Yummy sausages!   ");
  delay(3000);
}

void loop() {
  RTCDateTime dt = clock.getDateTime();
  updateLcd(lcd,
            "     The Box!       ",
            "   Digital Geyser   ",
            "                    ",
            clock.dateFormat("m/d/Y H:i", dt));
  delay(1000);
}
