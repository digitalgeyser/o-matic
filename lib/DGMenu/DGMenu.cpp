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
               uint8_t pin5) {
  lcd = new LiquidCrystal(pin0, pin1, pin2, pin3, pin4, pin5);
  lcd->begin(16,2);
}


void DGMenu::refresh(void)
{
  lcd->clear();
  lcd->print(line1);  
  lcd->setCursor(0,1);
  lcd->print(line2);
}

