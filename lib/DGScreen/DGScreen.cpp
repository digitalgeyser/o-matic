/*
 * (c) Digital Geyser.
 *
 * Screen library.
 */

#include "DGScreen.h"

DGScreen::DGScreen(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t reset) {
   tft = new Elegoo_TFTLCD(cs, cd, wr, rd, reset);
   verticalSeparation = 10;
   horizontalSeparation = 7;
   charSize = 2;
   nextCharX = 0;
   nextCharY = 0;
   currentPage = firstPage = new DGScreenPage("Home", WHITE, BLACK);
}

void DGScreen::setColor(DGColor fg, DGColor bg) {
  currentPage->setFg(fg);
  currentPage->setBg(bg);
}

void DGScreen::setFg(DGColor fg) {
  currentPage->setFg(fg);
}

void DGScreen::setBg(DGColor bg) {
  currentPage->setBg(bg);
}

void DGScreen::clearScreen() {
  fillScreen(currentPage->bg());
}

void DGScreen::clearRect(int16_t x, int16_t y, int16_t w, int16_t h) {
  fillRect(x, y, w, h, currentPage->bg());
}

int16_t DGScreen::width() { return tft->width(); }

int16_t DGScreen::height() { return tft->height(); }

void DGScreen::fillScreen(uint16_t color) {
  tft->fillScreen(color);
}

void DGScreen::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, DGColor color) {
  tft->fillRect(x, y, w, h, color);
}

void DGScreen::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, DGColor color) {
  tft->fillRoundRect(x, y, w, h, radius, color);
}

void DGScreen::fillCircle(int16_t x0, int16_t y0, int16_t r, DGColor color) {
  tft -> fillCircle(x0, y0, r, color);
}

void DGScreen::drawText(int x, int y, const char *txt, DGColor fg, DGColor bg) {
  DGColor currentFg = currentPage->fg();
  DGColor currentBg = currentPage->bg();
  setFg(fg);
  setBg(bg);
  drawText(x, y, txt);
  setFg(currentFg);
  setBg(currentBg);
}

void DGScreen::drawText(int x, int y, const char *txt, DGColor fg) {
  DGColor currentFg = currentPage->fg();
  setFg(fg);
  drawText(x, y, txt);
  setFg(currentFg);
}

void DGScreen::drawText(int x, int y, const char *txt) {
  int i, n = strlen(txt);
  fillRect(x, y, n*charSize*horizontalSeparation, charSize*verticalSeparation, currentPage->bg());
  for ( i = 0; i<n; i++ ) {
    drawChar(x+i*charSize*horizontalSeparation, y, txt[i]);
  }
}

void DGScreen::appendInt(int n, uint8_t digits) {
  int factor = 1;
  for ( int i=0; i<digits-1; i++ ) {
    factor *= 10;
  }
  while(factor >= 1) {
    appendChar('0' + (n/factor)%10);
    factor /= 10;
  }
}

void DGScreen::appendChar(unsigned char c) {
    drawChar(nextCharX, nextCharY, c);
}

void DGScreen::drawChar(int16_t x, int16_t y, unsigned char c) {
  tft -> drawChar(x, y, c, currentPage->fg(), currentPage->bg(), charSize);
  nextCharX = x+charSize*horizontalSeparation;
  nextCharY = y;
}

boolean DGScreen::processTouch(int16_t x, int16_t y) {
  switch(rotation) {
    case ROTATION_UPSIDE_DOWN:
      // For some reason this is normal for the touch.
      break;
    case ROTATION_NORMAL:
      x = width() - x;
      y = height() - y;
      break;
    case ROTATION_SIDEWAYS_LEFT:
      x = height() - y;
      y = x;
      break;
    case ROTATION_SIDEWAYS_RIGHT:
      x = y;
      y = width() - x;
      break;
  }
  DGScreenArea *finger = currentPage->area();
  while(finger != NULL ) {
    if ( x >= finger->x0 && x <= finger->x1 && y >= finger->y0 && y <= finger->y1 ) {
      if ( finger->callback != NULL ) {
        (*(finger->callback))();
        return true;
      }
    }
    finger = finger->nextArea;
  }
  return false;
}

void DGScreen::addButton(int16_t x0, int16_t y0, int16_t w, int16_t h, const char *txt, DGColor color, DGColor textColor, DGScreenCallback callback, boolean isHollow) {
  DGScreenArea *a = new DGScreenArea;
  a->x0 = x0;
  a->y0 = y0;
  a->x1 = x0+w;
  a->y1 = y0+h;
  a->callback = callback;
  currentPage->addArea(a);
  this->fillRoundRect(x0, y0, w, h, w/5, color);
  if ( isHollow ) {
    this->fillRoundRect(x0+5, y0+5, w-10, h-10, w/5, BLACK);
  }
  if ( txt != NULL ) {
    int oldCharSize = this->charSize;
    this->charSize = 1;
    this->drawText(x0+10, y0 + (h/2) - verticalSeparation/2, txt, textColor, (isHollow?BLACK:color));
    this->charSize = oldCharSize;
  }
}

void DGScreen::setup(DGColor fg, DGColor bg, uint8_t rotation) {

  setColor(fg, bg);

  tft->reset();

  uint16_t identifier = tft->readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101) {
    identifier=0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
    Serial.println(identifier, HEX);
  }

  tft->begin(identifier);
  tft->setRotation(rotation);
  this->rotation = rotation;
}
