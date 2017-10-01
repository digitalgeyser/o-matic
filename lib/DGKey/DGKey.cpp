/**
 * (c) Digital Geyser.
 *
 * Simple library for keypresses.
 */

#include "DGKey.h"

DGKey::DGKey() {
  kp = new Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
}

char DGKey::getKey() {
  return kp->getKey();
}
