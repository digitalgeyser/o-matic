/*
 * (c) Digital Geyser.
 *
 * Simple library to manage keys.
 */

#ifndef DGKey_h
#define DGKey_h

#include <Keypad.h>

class DGKey {
public:
  DGKey();
  char getKey();

private:
  Keypad *kp;
  static const byte ROWS = 4; //four rows
  static const byte COLS = 4; //four columns
  //define the symbols on the buttons of the keypads
  char hexaKeys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
  };
  byte rowPins[ROWS] = {A15, A14, A13, A12}; //connect to the row pinouts of the keypad
  byte colPins[COLS] = {A11, A10, A9, A8}; //connect to the column pinouts of the keypad


};

#endif
