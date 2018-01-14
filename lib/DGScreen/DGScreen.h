/*
 * (c) Digital Geyser.
 *
 * 2.8" TFT screen library
 */

#ifndef DGScreen_h
#define DGScreen_h

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

typedef uint16_t DGColor;

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

class DGScreen {
  public:
    DGScreen(uint8_t cs, uint8_t cd, uint8_t wr, uint8_t rd, uint8_t reset);
    int16_t width();
    int16_t height();
    void setup(DGColor fg, DGColor bg);
    void clearScreen();
    void fillScreen(DGColor color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void clearRect(int16_t x, int16_t y, int16_t w, int16_t h);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawChar(int16_t x, int16_t y, unsigned char c);
    void appendChar(unsigned char c);
    void appendInt(int n, uint8_t digits);
    void drawText(int x, int y, const char *txt);
    void drawText(int x, int y, const char *txt, DGColor fg);
    void setColor(DGColor fg, DGColor bg);
    void setBg(DGColor bg);
    void setFg(DGColor fg);

  private:
    Elegoo_TFTLCD *tft;
    uint8_t verticalSeparation;
    uint8_t horizontalSeparation;
    uint8_t charSize;
    DGColor fg, bg;
    uint16_t nextCharX, nextCharY;
};

#endif
