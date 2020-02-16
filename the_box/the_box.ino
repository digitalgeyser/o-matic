// Libraries used:
//
//  Adafruit unified sensor - by Adafruit
//  DHT Sensor Library - by Adafruit
//  RTClib - by Adafruit
//  LiquidCrystal_I2C - by Frank de Brabander
//  Button - by Michael Adams

#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Button.h>


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 30
#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 31

#define IN_SENSOR_TYPE DHT22
#define OUT_SENSOR_TYPE DHT22

#define PWM_FAN_1_PIN 10
#define PWM_FAN_2_PIN 9
int fan1Speed = 100; // 0 - 255
int fan2Speed = 100; // 0 - 255

#define BUTTON_COUNT 5
Button button_next(26);
Button button_sel(22);
Button button_inc(24);
Button button_before(25);
Button button_dec(23);

#define BUTTON_NEXT 0
#define BUTTON_BEFORE 1
#define BUTTON_INC 2
#define BUTTON_DEC 3
#define BUTTON_SEL 4

Button buttons[5] = { button_next, button_before, button_inc, button_dec, button_sel };
int buttonX[5] = { 12, 10, 11, 11, 11 };
int buttonY[5] = {  2,  2,  1,  3,  2 };

RTC_DS3231 clock;

DHT outsideSensor(TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, OUT_SENSOR_TYPE);
DHT insideSensor(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, IN_SENSOR_TYPE);

unsigned long lastClockTick = 0;

void updateLcd(LiquidCrystal_I2C lcd,
               const char *line0,
               const char *line1,
               const char *line2,
               const char *line3) {
  lcd.setCursor ( 0, 0 );
  lcd.print(line0);
  lcd.setCursor ( 0, 1 );
  lcd.print(line1);
  lcd.setCursor ( 0, 2 );
  lcd.print(line2);
  lcd.setCursor ( 0, 3 );
  lcd.print(line3);
}

void setup() {
  int i;

  Serial.begin(115200);
  Serial.println(F("Serial init."));

  
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
  updateLcd(lcd,
            "Out:      %      C  ",
            " In:      %      C  ",
            "                    ",
            "   Digital Geyser   ");
  Serial.println(F("LCD init."));

  clock.begin();
  
  // This line will set the time to whatever the time was when the sketch was compiled.
  // Take this out later.
  clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println(F("Clock init."));

  for ( i = 0; i<BUTTON_COUNT; i++ ) {
    buttons[i].begin();
  }

  outsideSensor.begin();
  insideSensor.begin();
  Serial.println(F("Sensor init."));

  pinMode(PWM_FAN_1_PIN, OUTPUT);
  analogWrite(PWM_FAN_1_PIN, fan1Speed);

  pinMode(PWM_FAN_2_PIN, OUTPUT);
  analogWrite(PWM_FAN_2_PIN, fan2Speed);
}


void sensorTick(unsigned long currentTime) {
  static unsigned long lastSensorTick = 0;

  if ( currentTime - lastSensorTick > 5000 ) {
    float h = outsideSensor.readHumidity();
    float t = outsideSensor.readTemperature();
    lcd.setCursor(5, 0);
    lcd.print(h);
    lcd.setCursor(12, 0);
    lcd.print(t);

    h = insideSensor.readHumidity();
    t = insideSensor.readTemperature();
    lcd.setCursor(5, 1);
    lcd.print(h);
    lcd.setCursor(12, 1);
    lcd.print(t);
    lastSensorTick = currentTime;
  }
}

void clockTick(unsigned long currentTime) {
  static unsigned long lastClockTick = 0;

  if ( currentTime - lastClockTick > 1000 ) {
    DateTime now = clock.now();
    char buf2[] = "MM/DD/YYYY  hh:mm:ss";
    lcd.setCursor ( 0, 3 );
    lcd.print(now.toString(buf2));
    lastClockTick = currentTime;
  }
}

void fan1Change(int change)
{
  fan1Speed += change;
  if (fan1Speed > 255)
    fan1Speed = 255;
  if(fan1Speed < 0 )
    fan1Speed = 0;
  analogWrite(PWM_FAN_1_PIN, fan1Speed);
}

void fan2Change(int change)
{
  fan2Speed += change;
  if (fan2Speed > 255)
    fan2Speed = 255;
  if(fan2Speed < 0 )
    fan2Speed = 0;
  analogWrite(PWM_FAN_2_PIN, fan2Speed);
}

void checkButtons() {
  for ( int i = 0; i<BUTTON_COUNT; i++ ) {
    if ( buttons[i].toggled() ) {
      lcd.setCursor(buttonX[i], buttonY[i]);
      if ( buttons[i].read() == Button::PRESSED ) {
        lcd.print("O");
        if ( i == BUTTON_INC ) {
          fan1Change(25);
        } else if ( i == BUTTON_DEC ) {
          fan1Change(-25);
        } else if ( i == BUTTON_NEXT ) {
          fan2Change(25);
        } else if ( i == BUTTON_BEFORE ) {
          fan2Change(-25);
        } else if ( i == BUTTON_SEL ) {
          if ( fan1Speed == 255 ) {
            fan1Change(-255);
            fan2Change(-255);
          } else {
            fan1Change(255);
            fan2Change(255);
          }
        }
      } else {
        lcd.print(" ");
      }     
    }
  }
}

void loop() {
  unsigned long currentTime = millis();
  checkButtons();

  sensorTick(currentTime);
  clockTick(currentTime);
}
