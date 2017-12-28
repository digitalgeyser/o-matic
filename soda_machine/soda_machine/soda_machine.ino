#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 We have only a single MAX72XX.
 */

// Arguments: DATA, CLK, CS, count
LedControl lc = LedControl(12,10,11,1);

byte smiley[8] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

byte SODA[][8]=
          {{ B01111110,
             B10000001,
             B00000001,
             B00001110,
             B01110000,
             B10000000,
             B10000001,
             B01111110},
            
            {B01111110,
             B10000001,
             B10000001,
             B10000001,
             B10000001,
             B10000001,
             B10000001,
             B01111110},
            
            {B00111111,
             B01000001,
             B10000001,
             B10000001,
             B10000001,
             B10000001,
             B01000001,
             B00111111},
            
            {B00011000,
             B00100100,
             B01000010,
             B10000001,
             B11111111,
             B10000001,
             B10000001,
             B10000001}};

byte soda[][8]=
          {{ B00000000,
             B00000000,
             B00000000,
             B00111100,
             B00000100,
             B00111100,
             B00100000,
             B00111100},
            
            {B00000000,
             B00000000,
             B00000000,
             B00111100,
             B00100100,
             B00100100,
             B00100100,
             B00111100},
            
            {B00100000,
             B00100000,
             B00100000,
             B00111000,
             B00100100,
             B00100100,
             B00100100,
             B00111000},
            
            {B00000000,
             B00000000,
             B00100000,
             B00111000,
             B00100100,
             B00100100,
             B00100100,
             B00111000}};
            

void setup() {
  pinMode(7, OUTPUT);
    /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

}

void loop() {
  writeLetters(soda);

  spiral(10, true);
  spiral(10, false);
  
  single(5, true);
  single(5, false);
  single(5, true);
  single(5, false);

  lines(1, 3, 35);
  lines(2, 3, 35);
  lines(3, 3, 35);
}

//We always have to include the library
#include "LedControl.h"

/* we always wait a bit between updates of the display */
unsigned long delaytime=500;

/*
 This method will display the characters for the
 word "Arduino" one after the other on the matrix. 
 (you need at least 5x7 leds to see the whole chars)
 */
void writeLetters(byte letters[][8]) {
  int i, j;

  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<8; j++ ) {
      lc.setColumn(0,j,letters[i][j]);      
    }
    delay(delaytime);    
  }
  for ( int j=0; j<8; j++ ) {
    lc.setColumn(0,j,0);      
  }
  delay(delaytime);
}

/* 
 This function will light up every Led on the matrix.
 The led will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void single(int delayMs, bool flag) {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      delay(delayMs);
      lc.setLed(0,row,col,flag);
    }
  }
}

void spiralXY(int n, int *x, int *y) {
  *x = n / 8;
  *y = n % 8;
}

void spiral(int delayMs, boolean on) {
  int x, y;
  for ( int n = 0; n<64; n++ ) {
    spiralXY(n, &x, &y);
    lc.setLed(0, x, y, on);
    delay(delayMs);
  }
}

void lines(int mask, int scanCount, int delayMs) {
  for ( int i=0; i<scanCount; i++) {
    
    for ( int row = 0; row<8; row++) {
    
      if ( mask & 1 ) lc.setRow(0, row, B11111111);
      if ( mask & 2 ) lc.setColumn(0, row, B11111111);
      
      delay(delayMs);

      if ( mask & 1 ) lc.setRow(0, row, B00000000);
      if ( mask & 2 ) lc.setColumn(0, row, B00000000);

    }
  
    for ( int row = 0; row<8; row++) {
    
      if ( mask & 1 ) lc.setRow(0, 7-row, B11111111);
      if ( mask & 2 ) lc.setColumn(0, 7-row, B11111111);
      delay(delayMs);
      if ( mask & 1 ) lc.setRow(0, 7-row, B00000000);
      if ( mask & 2 ) lc.setColumn(0, 7-row, B00000000);
    }
  }
}

