/*
 * (c) Digital Geyser.
 *
 * 2x16 LCD library.
 */

#ifndef DGMenu_h
#define DGMenu_h

#include <LiquidCrystal.h>
#define WIDTH 16
#define HEIGHT 2

class DGMenu {
  public:
    DGMenu(uint8_t pin0,
           uint8_t pin1,
           uint8_t pin2,
           uint8_t pin3,
           uint8_t pin4,
           uint8_t pin5,
           const char *line1,
           const char *line2);

  void refresh(void);

  void show(int columnIndex, int rowIndex, int width, int number);

  void show(int columnIndex, int rowIndex, char ch);

  void show(int columnIndex, int rowIndex, const char *text);

  void screen(const char *line1, const char *line2);

  void clear();

  void reinit();

 private:
  LiquidCrystal *lcd;
  char row[2][16];

};

#endif
