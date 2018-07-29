/*
 * (c) Digital Geyser.
 *
 * Screen library.
 */

#include "DGScreen.h"

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define TS_CONTROL_PIN 13

#define MINPRESSURE 10
#define MAXPRESSURE 1000

DGScreen::DGScreen(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t reset) {
   tft = new Elegoo_TFTLCD(cs, cd, wr, rd, reset);

   // For better pressure precision, we need to know the resistance
   // between X+ and X- Use any multimeter to read it
   // For the one we're using, its 300 ohms across the X plate
   ts = new TouchScreen(XP, YP, XM, YM, 300);
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

void DGScreen::touchScreen() {
  digitalWrite(TS_CONTROL_PIN, HIGH);
  TSPoint p = ts->getPoint();
  digitalWrite(TS_CONTROL_PIN, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    processTouch(p.x, p.y);
  }
}

boolean DGScreen::processTouch(int16_t x, int16_t y) {
  switch(rotation) {
    int16_t tmp;
    case ROTATION_NORMAL:
      x = map(x, TS_MINX, TS_MAXX, 0, width());
      y = map(y, TS_MINY, TS_MAXY, height(), 0);
      break;
    case ROTATION_UPSIDE_DOWN:
      x = map(x, TS_MINX, TS_MAXX, width(), 0);
      y = map(y, TS_MINY, TS_MAXY, 0, height());
      break;
    case ROTATION_SIDEWAYS_RIGHT:
      tmp = y;
      y = map(x, TS_MINX, TS_MAXX, height(), 0);
      x = map(tmp, TS_MINY, TS_MAXY, width(), 0);
      break;
    case ROTATION_SIDEWAYS_LEFT:
      tmp = y;
      y = map(x, TS_MINX, TS_MAXX, 0, height());
      x = map(tmp, TS_MINY, TS_MAXY, 0, width());
      break;
  }
  DGScreenButton *finger = currentPage->firstButton();
  while(finger != NULL ) {
    if ( x >= finger->x0 && x <= finger->x1 && y >= finger->y0 && y <= finger->y1 ) {
      if ( finger->callback != NULL ) {
        (*(finger->callback))();
        return true;
      }
    }
    finger = finger->nextButton;
  }
  return false;
}

void DGScreen::addButton(int16_t x0,
                         int16_t y0,
                         int16_t w,
                         int16_t h,
                         const char *txt,
                         DGColor color,
                         DGColor textColor,
                         DGScreenCallback callback,
                         boolean isHollow) {
  DGScreenButton *a = new DGScreenButton;
  a->x0 = x0;
  a->y0 = y0;
  a->x1 = x0+w;
  a->y1 = y0+h;
  a->callback = callback;
  currentPage->addButton(a);
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
