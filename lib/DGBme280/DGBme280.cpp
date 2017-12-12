#include "DGBme280.h"
#include <math.h>
#include <Wire.h>

// This code is a fork of cactus_io_BME280_I2C library, repurposed
// for the HiLetgo BME280 module.

DGBme280::DGBme280(void) {
  _i2caddr = BME280_ADDRESS;
  tempcal = 0.0;
  temperature = 0.0;
  humidity = 0.0;
  pressure = 0.0;
}

DGBme280::DGBme280(uint8_t addr) {
  _i2caddr = addr;
  tempcal = 0.0;
  tempcal = 0.0;
  temperature = 0.0;
  humidity = 0.0;
}

void DGBme280::setTempCal(float tcal) {
  tempcal = tcal;
}

void DGBme280::readSensor(void) {
  readTemperature();
  readHumidity();
  readPressure();
}

float DGBme280::getTemperature_C(void) {
  temperature = temperature + tempcal;
  return temperature;
}

float DGBme280::getTemperature_F(void) {
  temperature = temperature + tempcal;
  return temperature * 1.8 + 32;
}

float DGBme280::getHumidity(void) {
  return humidity;
}

// Gets the pressure in millibars
float DGBme280::getPressure_MB(void) {
  return pressure / 100.0F;
}

// Gets the pressure in hectapascals
float DGBme280::getPressure_HP(void) {
  return pressure;
}

bool DGBme280::begin() {
  int chipid;
  Wire.begin();
  chipid = read8(BME280_REGISTER_CHIPID);
  Serial.println(chipid);
  if (chipid!= 88)
    return false;
  readSensorCoefficients();
  write8(BME280_REGISTER_CONTROLHUMID, 0x01);       // Set before CONTROL (DS 5.4.3)
  write8(BME280_REGISTER_CONTROL, 0x3F);
  return true;
}

void DGBme280::readTemperature(void) {
  int32_t var1, var2;
  int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
  adc_T >>= 4;
  var1  = ((((adc_T>>3) - ((int32_t)cal_data.dig_T1 <<1))) *
           ((int32_t)cal_data.dig_T2)) >> 11;
  var2  = (((((adc_T>>4) - ((int32_t)cal_data.dig_T1)) *
             ((adc_T>>4) - ((int32_t)cal_data.dig_T1))) >> 12) *
           ((int32_t)cal_data.dig_T3)) >> 14;
  t_fine = var1 + var2;
  temperature  = (t_fine * 5 + 128) >> 8;
  temperature = temperature / 100;
}


void DGBme280::readPressure(void) {
  int64_t var1, var2, p;
  int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)cal_data.dig_P6;
  var2 = var2 + ((var1*(int64_t)cal_data.dig_P5)<<17);
  var2 = var2 + (((int64_t)cal_data.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)cal_data.dig_P3)>>8) +
         ((var1 * (int64_t)cal_data.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)cal_data.dig_P1)>>33;
  if (var1 == 0) {
    // return 0;  // avoid exception caused by division by zero
    pressure = 0.0;
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)cal_data.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)cal_data.dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)cal_data.dig_P7)<<4);
  // return (float)p/256;
  pressure = (float)p/256;
}


void DGBme280::readHumidity(void) {
  int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
  int32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((int32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)cal_data.dig_H4) << 20) -
                  (((int32_t)cal_data.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
               (((((((v_x1_u32r * ((int32_t)cal_data.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)cal_data.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)cal_data.dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)cal_data.dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  // return  h / 1024.0;
  humidity = h / 1024.0;
}

/**************************************************************************
   Read the values that are programmed into the sensor during amanufacture
**************************************************************************/

void DGBme280::readSensorCoefficients(void) {
  cal_data.dig_T1 = read16_LE(BME280_DIG_T1_REG);
  cal_data.dig_T2 = readS16_LE(BME280_DIG_T2_REG);
  cal_data.dig_T3 = readS16_LE(BME280_DIG_T3_REG);
  cal_data.dig_P1 = read16_LE(BME280_DIG_P1_REG);
  cal_data.dig_P2 = readS16_LE(BME280_DIG_P2_REG);
  cal_data.dig_P3 = readS16_LE(BME280_DIG_P3_REG);
  cal_data.dig_P4 = readS16_LE(BME280_DIG_P4_REG);
  cal_data.dig_P5 = readS16_LE(BME280_DIG_P5_REG);
  cal_data.dig_P6 = readS16_LE(BME280_DIG_P6_REG);
  cal_data.dig_P7 = readS16_LE(BME280_DIG_P7_REG);
  cal_data.dig_P8 = readS16_LE(BME280_DIG_P8_REG);
  cal_data.dig_P9 = readS16_LE(BME280_DIG_P9_REG);
  cal_data.dig_H1 = read8(BME280_DIG_H1_REG);
  cal_data.dig_H2 = readS16_LE(BME280_DIG_H2_REG);
  cal_data.dig_H3 = read8(BME280_DIG_H3_REG);
  cal_data.dig_H4 = (read8(BME280_DIG_H4_REG) << 4) | (read8(BME280_DIG_H4_REG+1) & 0xF);
  cal_data.dig_H5 = (read8(BME280_DIG_H5_REG+1) << 4) | (read8(BME280_DIG_H5_REG) >> 4);
  cal_data.dig_H6 = (int8_t)read8(BME280_DIG_H6_REG);
}



/**************************************************************************

   Writes an 8 bit value over I2C

**************************************************************************/

void DGBme280::write8(byte reg, byte value) {
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)value);
  Wire.endTransmission();
}

/**************************************************************************

   Reads a signed 8 bit value over the I2C bus_REG

**************************************************************************/

uint8_t DGBme280::read8(byte reg) {
  uint8_t value;
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)1);
  value = Wire.read();
  return value;
}


/**************************************************************************

   Reads a signed 16 bit value over the I2C bus_REG

**************************************************************************/

int16_t DGBme280::readS16(byte reg) {
  return (int16_t)read16(reg);
}

int16_t DGBme280::readS16_LE(byte reg) {
  return (int16_t)read16_LE(reg);
}

uint16_t DGBme280::read16(byte reg) {
  uint16_t value;
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)2);
  value = (Wire.read() << 8) | Wire.read();
  return value;
}

uint16_t DGBme280::read16_LE(byte reg) {
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
}

/**************************************************************************

   Reads a signed 24 bit value over the I2C bus_REG

**************************************************************************/

uint32_t DGBme280::read24(byte reg) {
  uint32_t value;
  Wire.beginTransmission((uint8_t)_i2caddr);
  Wire.write((uint8_t)reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)_i2caddr, (byte)3);
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  value <<= 8;
  value |= Wire.read();
  return value;
}
