#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 We have only a single MAX72XX.
 */

// Arguments: DATA, CLK, CS, count
LedControl lc = LedControl(12,10,11,1);

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
  delay(500);
  digitalWrite(7, HIGH);
  delay(500);
  digitalWrite(7, LOW);
  writeArduinoOnMatrix();
  single(50, true);
  single(50, false);
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
void writeArduinoOnMatrix() {
  int i, j;
  /* here is the data for the characters */
  byte a[][5]={{B01111110,
             B10001000,
             B10001000,
             B10001000,
             B01111110},
            {B00111110,
             B00010000,
             B00100000,
             B00100000,
             B00010000},
            {B00011100,
             B00100010,
             B00100010,
             B00010010,
             B11111110},
            {B00111100,
             B00000010,
             B00000010,
             B00000100,
             B00111110},
            {B00000000,
             B00100010,
             B10111110,
             B00000010,
             B00000000},
            {B00111110,
             B00010000,
             B00100000,
             B00100000,
             B00011110},
            {B00011100,
             B00100010,
             B00100010,
             B00100010,
             B00011100}};

  for ( int i=0; i<7; i++ ) {
    for ( int j=0; j<5; j++ ) {
      lc.setColumn(0,j,a[i][j]);      
    }
    delay(delaytime);    
  }
  for ( int j=0; j<5; j++ ) {
    lc.setColumn(0,j,0);      
  }
  delay(delaytime);
}

/*
  This function lights up a some Leds in a row.
 The pattern will be repeated on every row.
 The pattern will blink along with the row-number.
 row number 4 (index==3) will blink 4 times etc.
 */
void rows() {
  for(int row=0;row<8;row++) {
    delay(delaytime);
    lc.setRow(0,row,B10100000);
    delay(delaytime);
    lc.setRow(0,row,(byte)0);
    for(int i=0;i<row;i++) {
      delay(delaytime);
      lc.setRow(0,row,B10100000);
      delay(delaytime);
      lc.setRow(0,row,(byte)0);
    }
  }
}

/*
  This function lights up a some Leds in a column.
 The pattern will be repeated on every column.
 The pattern will blink along with the column-number.
 column number 4 (index==3) will blink 4 times etc.
 */
void columns() {
  for(int col=0;col<8;col++) {
    delay(delaytime);
    lc.setColumn(0,col,B10100000);
    delay(delaytime);
    lc.setColumn(0,col,(byte)0);
    for(int i=0;i<col;i++) {
      delay(delaytime);
      lc.setColumn(0,col,B10100000);
      delay(delaytime);
      lc.setColumn(0,col,(byte)0);
    }
  }
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

