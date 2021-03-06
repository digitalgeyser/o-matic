/*
 * (c) Digital Geyser.
 *
 * Screen library.
 */

#ifndef DGScreen_h
#define DGScreen_h

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#include <TouchScreen.h>

typedef uint16_t DGColor;

#define ROTATION_NORMAL 0
#define ROTATION_SIDEWAYS_RIGHT 1
#define ROTATION_UPSIDE_DOWN 2
#define ROTATION_SIDEWAYS_LEFT 3

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

typedef void (*DGScreenCallback)();

typedef struct _DGScreenButton {
  uint16_t x0;
  uint16_t y0;
  uint16_t x1;
  uint16_t y1;
  DGScreenCallback callback;
  struct _DGScreenButton *nextButton;
} DGScreenButton;

class DGScreenPage {
  public:
    DGScreenPage(const char *title, DGColor fg, DGColor bg);
    DGScreenButton *firstButton();
    void addButton(DGScreenButton *button);
    void setBg(DGColor bg);
    void setFg(DGColor fg);
    DGColor fg();
    DGColor bg();
  private:
    const char *title;
    DGColor bgColor, fgColor;
    DGScreenPage *nextPage;
    DGScreenButton *buttonFirst;
};

class DGScreen {
  public:
    DGScreen(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t reset);
    DGScreenPage *createPage();
    int16_t width();
    int16_t height();
    void setup(DGColor fg, DGColor bg, uint8_t rotation);
    void clearScreen();
    void fillScreen(DGColor color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, DGColor color);
    void clearRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawChar(int16_t x, int16_t y, unsigned char c);
    void appendChar(unsigned char c);
    void appendInt(int n, uint8_t digits);
    void drawText(int x, int y, const char *txt);
    void drawText(int x, int y, const char *txt, DGColor fg);
    void drawText(int x, int y, const char *txt, DGColor fg, DGColor bg);
    void setColor(DGColor fg, DGColor bg);
    void setBg(DGColor bg);
    void setFg(DGColor fg);
    void touchScreen();
    boolean processTouch(int16_t x, int16_t y);
    void addButton(int16_t x0, int16_t y0, int16_t w, int16_t h, const char *txt, DGColor color, DGColor textColor, DGScreenCallback callback, boolean isHollow);

  private:
    Elegoo_TFTLCD *tft;
    TouchScreen *ts;
    uint8_t verticalSeparation;
    uint8_t horizontalSeparation;
    uint8_t charSize;
    uint16_t nextCharX, nextCharY;
    DGScreenPage *firstPage, *currentPage;
    uint8_t rotation = ROTATION_NORMAL;
};

#endif
