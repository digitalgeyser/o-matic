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
  strncpy(row0, line1, WIDTH);
  strncpy(row1, line2, WIDTH);
}

char *DGMenu::line1() {
  return row0;
}

char *DGMenu::line2() {
  return row1;
}

void DGMenu::refresh(void) {
  lcd->clear();
  
  lcd->setCursor(0,0);
  lcd->print(row0);  

  lcd->setCursor(0,1);
  lcd->print(row1);
}

void DGMenu::show(int column, int row, int number, int width) {
  char *line;
  int i;
  if ( row == 0 ) {
    line = row0;
  } else {
    line = row1;
  }
  for ( i = 0; i<width; i++ ) {
    int index = column+width-i-1;
    if ( index >=0 && index < WIDTH )
      line[index] = '0' + (number%10);
    number = number / 10;
  }
}
