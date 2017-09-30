/*
 * (c) Digital Geyser.
 *
 * 2x16 LCD library.
 */

// ensure this library description is only included once
#ifndef DGMenu_h
#define DGMenu_h

#include <LiquidCrystal.h>
#define WIDTH 16
#define HEIGHT 2

class DGMenu {
  // user-accessible "public" interface
 public:
  DGMenu(uint8_t pin0, uint8_t pin1,
	 uint8_t pin2, uint8_t pin3,
	 uint8_t pin4, uint8_t pin5,
	 const char *line1, const char *line2);
  
  void refresh(void);
  
  void show(int columnIndex, int rowIndex, int width, int number);

  void show(int columnIndex, int rowIndex, char ch);

  void screen(const char *line1, const char *line2);

  void clear();
  
 private:
  LiquidCrystal *lcd;
  char* row[2] = { "0123456789012345", "5432109876543210" };
  
};

#endif

