#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 We have only a single MAX72XX.
 */

// Rotary
const int PinCLK=2;   // Generating interrupts using CLK signal
const int PinDT=3;    // Reading DT signal
const int PinSW=4;    // Reading Push Button switch

volatile boolean TurnDetected;  // need volatile for Interrupts
volatile boolean rotationdirection;  // CW or CCW rotation

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

#define CIRCLE_COUNT 4
byte CIRCLE[][8]=
{
  { B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000,
    B00000000 },

  { B00000000,
    B00000000,
    B00011000,
    B00111100,
    B00111100,
    B00011000,
    B00000000,
    B00000000 },
  
  { B00000000,
    B00111100,
    B00111110,
    B01111110,
    B01111110,
    B01111110,
    B00111100,
    B00000000 },
  
  { B00111100,
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100 },
  
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
            
void isr ()  {
  delay(4);  // delay for Debouncing
  if (digitalRead(PinCLK))
    rotationdirection= digitalRead(PinDT);
  else
    rotationdirection= !digitalRead(PinDT);
  TurnDetected = true;
}

void setup() {
  pinMode(7, OUTPUT);
  pinMode(PinCLK,INPUT);
  pinMode(PinDT,INPUT);  
  pinMode(PinSW,INPUT);
  digitalWrite(PinSW, HIGH); // Pull-Up resistor for switch
  attachInterrupt (0,isr,FALLING); // interrupt 0 always connected to pin 2 on Arduino UNO

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

void checkRotary() {
  bool sw = (!digitalRead(PinSW));
  if (sw || TurnDetected ) {
    lc.setLed(0,0,0,sw);
    lc.setLed(0,0,rotationdirection?1:2,TurnDetected);
    TurnDetected = false;
    delay(500);
  }
}

void loop() {
  checkRotary();
  
  writeLetters(soda);
  checkRotary();

  spiral(20, true);
  checkRotary();
  spiral(20, false);
  checkRotary();

  circle(20, 5);
  checkRotary();
  
  single(5, true);
  checkRotary();
  single(5, false);
  checkRotary();
  single(5, true);
  checkRotary();
  single(5, false);
  checkRotary();

  lines(1, 3, 35);
  checkRotary();
  lines(2, 3, 35);
  checkRotary();
  lines(3, 3, 35);
  checkRotary();
}

//We always have to include the library
#include "LedControl.h"

/* we always wait a bit between updates of the display */
unsigned long delaytime=500;

void drawPattern(byte pattern[8]) {
  for ( int j=0; j<8; j++ ) {
    lc.setColumn(0,j,pattern[j]);      
  }  
}

void clear() {
  for ( int j=0; j<8; j++ ) {
    lc.setColumn(0,j,0);      
  }  
}
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
  clear();
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

void circle(int delayMs, int count) {
  for ( int i=0; i<count; i++ ) {
    for ( int j=0; j<CIRCLE_COUNT; j++ ) {
      drawPattern(CIRCLE[j]);
      delay(delayMs);
    }
  }
  clear();
}

// call spiral(NULL, NULL) to reset.
void do_spiral(int *x, int *y) {
  static int n, lastX, lastY;
  static int dir, step;

  if ( x == NULL || y == NULL ) {
    n = 0;
    step = 0;
    return;
  }

  if ( step == 0 && n == 0 ) {
    // First step.
    lastX = 0;
    lastY = 0;
    dir = 0;
    step = 1;
  } else {

    // Make a move
    switch(dir) {
    case 0: lastX++; break;
    case 1: lastY++; break;
    case 2: lastX--; break;
    case 3: lastY--; break;
    }
    step--;

    if ( step == 0 ) {
      dir = (dir+1)%4;
      n++;
      step = n/2 + 1;
    }
  }

  *x = lastX;
  *y = lastY;
}

void spiral(int delayMs, boolean on) {
  do_spiral(NULL, NULL);
  for ( int n = 0; n<64; n++ ) {
    int x, y;
    do_spiral(&x, &y);
    lc.setLed(0, 3+x, 3+y, on);
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

