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


/********************** CLOCK **************************/
RTC_DS3231 clock;
unsigned long lastClockTick = 0;

/********************** SENSORS *************************/
#define TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN 30
#define TEMPERATURE_HUMIDITY_SENSOR_IN_PIN 31

#define IN_SENSOR_TYPE DHT22
#define OUT_SENSOR_TYPE DHT22
DHT outsideSensor(TEMPERATURE_HUMIDITY_SENSOR_OUT_PIN, OUT_SENSOR_TYPE);
DHT insideSensor(TEMPERATURE_HUMIDITY_SENSOR_IN_PIN, IN_SENSOR_TYPE);


/********************** BUTTONS *************************/
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

Button buttons[5] = {button_next, button_before, button_inc, button_dec, button_sel};
int buttonX[5] = {12, 10, 11, 11, 11};
int buttonY[5] = {2, 2, 1, 3, 2};

void initButtons() {
  int i;
  for (i = 0; i < BUTTON_COUNT; i++)
  {
    buttons[i].begin();
  }
}

/********************** PELTIER OPERATIONS **************/
#define PELTIER_RELAY_0_PIN 34
#define PELTIER_RELAY_1_PIN 35

#define PELTIER_INIT 0
#define PELTIER_HEAT 1
#define PELTIER_COOL 2
#define PELTIER_OFF 3

void peltier(byte opt) {
  static int state;
  switch(opt) {
    case PELTIER_INIT:
      pinMode(PELTIER_RELAY_0_PIN, OUTPUT);
      pinMode(PELTIER_RELAY_1_PIN, OUTPUT);
      state = 0;
      break;
    case PELTIER_OFF:
      state = 0;
      break;
    case PELTIER_HEAT:
      state = 0x01;
      break;
    case PELTIER_COOL:
      state = 0x02;
      break;
  }

  digitalWrite(PELTIER_RELAY_1_PIN, state & 0x01 ? LOW: HIGH);
  digitalWrite(PELTIER_RELAY_0_PIN, state & 0x02 ? LOW: HIGH);
}

/********************** FAN OPERATIONS ******************/
#define FAN_INIT 0
#define FAN_0_SET 1
#define FAN_1_SET 2
#define FAN_0_CHANGE 3
#define FAN_1_CHANGE 4
#define FAN_TOGGLE_MIN_MAX 5

#define PWM_FAN_0_PIN 10
#define PWM_FAN_1_PIN 9

#define FAN_0_TACH_PIN 2
#define FAN_1_TACH_PIN 3

volatile int fan0PulseCounter, fan1PulseCounter;

void fan0Tach() {
  fan0PulseCounter++;
}

void fan1Tach() {
  fan1PulseCounter++;
}

/**
 * Performs a fan operation.
 * Opt can be one of the operations above.
 * Arg is a number 0-255.
 */
void fan(byte opt, int arg)
{
  static int fanSpeed[] = {100, 100};
  int ch = 0; // bitmask tracking which fans changed

  switch (opt)
  {
  case FAN_INIT:
    pinMode(PWM_FAN_0_PIN, OUTPUT);
    pinMode(PWM_FAN_1_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(FAN_0_TACH_PIN), fan0Tach, RISING);
    attachInterrupt(digitalPinToInterrupt(FAN_1_TACH_PIN), fan1Tach, RISING);
    ch |= (0x01 | 0x02);
    break;
  case FAN_0_SET:
    fanSpeed[0] = arg;
    ch |= 0x01;
    break;
  case FAN_1_SET:
    fanSpeed[1] = arg;
    ch |= 0x02;
    break;
  case FAN_0_CHANGE:
    fanSpeed[0] += arg;
    ch |= 0x01;
    if (fanSpeed[0] > 255)
      fanSpeed[0] = 255;
    if (fanSpeed[0] < 0)
      fanSpeed[0] = 0;
    break;
  case FAN_1_CHANGE:
    fanSpeed[1] += arg;
    ch |= 0x02;
    if (fanSpeed[1] > 255)
      fanSpeed[1] = 255;
    if (fanSpeed[1] < 0)
      fanSpeed[1] = 0;
    break;
  case FAN_TOGGLE_MIN_MAX:
    if (fanSpeed[0] != 255)
    {
      fanSpeed[0] = 255;
      fanSpeed[1] = 255;
    }
    else
    {
      fanSpeed[0] = 0;
      fanSpeed[1] = 0;
    }
    ch |= (0x01 | 0x02);
    break;
  }
  if (ch & 0x01)
    analogWrite(PWM_FAN_0_PIN, fanSpeed[0]);
  if (ch & 0x02)
    analogWrite(PWM_FAN_1_PIN, fanSpeed[1]);
}

void fanTick(unsigned long currentTime)
{
  static unsigned long lastFanTick = 0;

  if ( (currentTime - lastFanTick) > 1000) {
    fan0PulseCounter = fan1PulseCounter = 0;
    delay(100);
    lcdPrintIntAt(5, 2, fan0PulseCounter); // At full speed, this is mostly 5, sometimes 4.
    lcdPrintIntAt(12, 2, fan1PulseCounter); // When fan is not spinning, this is 0.
    lastFanTick = currentTime;
  }
}

/******************* LCD OPERATIONS *********************/
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

void lcdUpdate(const char *line0, const char *line1, const char *line2, const char *line3)
{
  lcd.setCursor(0, 0);
  lcd.print(line0);
  lcd.setCursor(0, 1);
  lcd.print(line1);
  lcd.setCursor(0, 2);
  lcd.print(line2);
  lcd.setCursor(0, 3);
  lcd.print(line3);
}

void lcdPrintFloatAt(byte x, byte y, float f)
{
  lcd.setCursor(x, y);
  lcd.print(f);
}

void lcdPrintIntAt(byte x, byte y, int i) {
  lcd.setCursor(x, y);
  lcd.print(i);
}

void lcdPrintStringAt(byte x, byte y, const char *s)
{
  lcd.setCursor(x, y);
  lcd.print(s);
}

void lcdInit()
{

  lcd.init();
  lcd.backlight();
  lcdUpdate("Out:      %      C  ",
            " In:      %      C  ",
            "Fan:                 ",
            "   Digital Geyser   ");
}

/******************* SETUP CODE ************************/

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Serial init."));

  lcdInit();
  
  clock.begin();

  // This line will set the time to whatever the time was when the sketch was compiled.
  // Take this out later.
  clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println(F("Clock init."));

  initButtons();
  outsideSensor.begin();
  insideSensor.begin();
  Serial.println(F("Sensor init."));

  peltier(PELTIER_INIT);
  fan(FAN_INIT, 0);
}

/******************** TICK CODE **********************/

void sensorTick(unsigned long currentTime)
{
  static unsigned long lastSensorTick = 0;

  if (currentTime - lastSensorTick > 5000)
  {
    float h = outsideSensor.readHumidity();
    float t = outsideSensor.readTemperature();
    lcdPrintFloatAt(5, 0, h);
    lcdPrintFloatAt(12, 0, t);

    h = insideSensor.readHumidity();
    t = insideSensor.readTemperature();
    lcdPrintFloatAt(5, 1, h);
    lcdPrintFloatAt(12, 1, t);
    lastSensorTick = currentTime;
  }
}

void clockTick(unsigned long currentTime)
{
  static unsigned long lastClockTick = 0;

  if (currentTime - lastClockTick > 1000)
  {
    DateTime now = clock.now();
    char buf2[] = "MM/DD/YYYY  hh:mm:ss";
    lcdPrintStringAt(0, 3, now.toString(buf2));
    lastClockTick = currentTime;
  }
}

void checkButtons()
{
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    if (buttons[i].toggled())
    {
      if (buttons[i].read() == Button::PRESSED)
      {
        lcdPrintStringAt(buttonX[i], buttonY[i], "O");
        if (i == BUTTON_INC)
        {
          fan(FAN_0_CHANGE, 25);
          fan(FAN_1_CHANGE, 25);
        }
        else if (i == BUTTON_DEC)
        {
          fan(FAN_0_CHANGE, -25);
          fan(FAN_1_CHANGE, -25);
        }
        else if (i == BUTTON_NEXT)
        {
          peltier(PELTIER_HEAT);
        }
        else if (i == BUTTON_BEFORE)
        {
          peltier(PELTIER_COOL);
        }
        else if (i == BUTTON_SEL)
        {
          fan(FAN_TOGGLE_MIN_MAX, 0);
        }
      }
      else
      {
        lcdPrintStringAt(buttonX[i], buttonY[i], " ");
      }
    }
  }
}

void loop()
{
  unsigned long currentTime = millis();
  checkButtons();

  sensorTick(currentTime);
  clockTick(currentTime);
  fanTick(currentTime);
}
