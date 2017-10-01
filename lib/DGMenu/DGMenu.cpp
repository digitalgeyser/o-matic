/*
 * (c) Digital Geyser.
 *
 * 2x16 LCD library.
 */

#include "DGMenu.h"

#include <LiquidCrystal.h>

DGMenu::DGMenu(uint8_t pin0,
               uint8_t pin1,
               uint8_t pin2,
               uint8_t pin3,
               uint8_t pin4,
               uint8_t pin5,
	       const char *line1,
	       const char *line2) {
  lcd = new LiquidCrystal(pin0, pin1, pin2, pin3, pin4, pin5);
  lcd->begin(WIDTH,HEIGHT);
  screen(line1, line2);
}

void DGMenu::refresh(void) {
  lcd->clear();

  lcd->setCursor(0,0);
  lcd->print(row[0]);

  lcd->setCursor(0,1);
  lcd->print(row[1]);
}

void DGMenu::clear() {
  memset(row[0], ' ', WIDTH);
  memset(row[1], ' ', WIDTH);
}

void DGMenu::screen(const char *line1, const char *line2) {
  strncpy(row[0], line1, WIDTH);
  strncpy(row[1], line2, WIDTH);
}

void DGMenu::show(int columnIndex, int rowIndex, int number, int width) {
  int i;
  for ( i = 0; i<width; i++ ) {
    int index = columnIndex+width-i-1;
    if ( index >=0 && index < WIDTH )
      row[rowIndex][index] = '0' + (number%10);
    number = number / 10;
  }
}

void DGMenu::show(int columnIndex, int rowIndex, char ch) {
  row[rowIndex][columnIndex] = ch;
}

void DGMenu::show(int columnIndex, int rowIndex, const char *text) {
  strncpy(&(row[rowIndex][columnIndex]), text, strlen(text));
}
