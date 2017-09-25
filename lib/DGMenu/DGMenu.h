/*
 * (c) Digital Geyser.
 *
 * 2x16 LCD library.
 */

// ensure this library description is only included once
#ifndef DGMenu_h
#define DGMenu_h

#include <LiquidCrystal.h>

#define NAME "Cure-O-Matic2000"
#define DG   "(c)DigitalGeyser"

class DGMenu {
  // user-accessible "public" interface
  public:
    DGMenu(uint8_t pin0, 
           uint8_t pin1,
           uint8_t pin2,
           uint8_t pin3,
           uint8_t pin4,
           uint8_t pin5);
    void refresh(void);

  private:
    LiquidCrystal *lcd;
    char *line1 =  NAME;
    char *line2 =  DG;

};

#endif

