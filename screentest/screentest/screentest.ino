#include <DGScreen.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4


DGScreen s(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
int c = 0;

void setup() {
  s.setup(GREEN, BLACK, ROTATION_SIDEWAYS_LEFT);
  s.clearScreen();
  s.addButton(0, 0, 80, 80, "Cnt", BLUE, BLACK, button, true);
}

void button() {
  c++;  
  s.drawText(110, 110, "Cnt:");
  s.appendInt(c, 4);
  s.drawText(110, 130, "W:");
  s.appendInt(s.width(), 3);
  s.drawText(110, 150, "H:");
  s.appendInt(s.height(), 3);
}

void loop() {
    readTouchScreen();
}

void readTouchScreen() {
  s.touchScreen();
}

