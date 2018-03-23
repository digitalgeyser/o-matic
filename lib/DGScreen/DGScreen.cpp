/*
 * (c) Digital Geyser.
 *
 * 2x16 LCD library.
 */

#include "DGScreen.h"

DGScreen::DGScreen(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t reset) {
   tft = new Elegoo_TFTLCD(cs, cd, wr, rd, reset);
   verticalSeparation = 20;
   horizontalSeparation = 13;
   charSize = 2;
   fg = WHITE;
   bg = BLACK;
   nextCharX = 0;
   nextCharY = 0;
   area = NULL;
}

void DGScreen::setColor(DGColor fg, DGColor bg) {
  this->fg = fg;
  this->bg = bg;
}

void DGScreen::setFg(DGColor fg) {
  this->fg = fg;
}

void DGScreen::setBg(DGColor bg) {
  this->bg = bg;
}

void DGScreen::clearScreen() {
  fillScreen(bg);
}

void DGScreen::clearRect(int16_t x, int16_t y, int16_t w, int16_t h) {
  fillRect(x, y, w, h, bg);
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

void DGScreen::drawText(int x, int y, const char *txt, DGColor fg) {
  DGColor currentFg = this->fg;
  setFg(fg);
  drawText(x, y, txt);
  setFg(currentFg);
}

void DGScreen::drawText(int x, int y, const char *txt) {
  int i, n = strlen(txt);
  fillRect(x, y, n*horizontalSeparation, verticalSeparation, bg);
  for ( i = 0; i<n; i++ ) {
    drawChar(x+i*horizontalSeparation, y, txt[i]);
  }
}

void DGScreen::appendInt(int n, uint8_t digits) {
  appendChar('0' + (n/100)%10);
  appendChar('0' + (n/10)%10);
  appendChar('0' + (n)%10);
}

void DGScreen::appendChar(unsigned char c) {
    drawChar(nextCharX, nextCharY, c);
}

void DGScreen::drawChar(int16_t x, int16_t y, unsigned char c) {
  tft -> drawChar(x, y, c, fg, bg, charSize);
  nextCharX = x+horizontalSeparation;
  nextCharY = y;
}

boolean DGScreen::processTouch(int16_t x, int16_t y) {
  DGScreenArea *finger = this->area;
  while(finger != NULL ) {
    if ( x >= finger->x0 && x <= finger->x1 && y >= finger->y0 && y <= finger->y1 ) {
      if ( finger->callback != NULL ) {
        (*(finger->callback))();
        return true;
      }
    }
    finger = finger->next;
  }
  return false;
}

void DGScreen::addButton(int16_t x0, int16_t y0, int16_t w, int16_t h, DGColor color, DGScreenCallback callback) {
  DGScreenArea *a = new DGScreenArea;
  a->x0 = x0;
  a->y0 = y0;
  a->x1 = x0+w;
  a->y1 = y0+h;
  a->callback = callback;
  a->next = this->area;
  this->area = a;
  this->fillRoundRect(x0, y0, w, h, w/5, color);
}


void DGScreen::setup(DGColor fg, DGColor bg) {

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
  tft->setRotation(2);

}
