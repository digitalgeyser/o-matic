#include <DS3231.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

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
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  updateLcd(lcd,
            "     The Box!       ",
            "   Digital Geyser   ",
            "                    ",
            "  Yummy sausages!   ");
}

void loop() {
}
