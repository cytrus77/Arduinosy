#ifndef SENSORS_H
#define SENSORS_H

#include <Adafruit_BMP085.h>
#include <Wire.h>


namespace sensors
{

struct BmpVal
{
  float temp;
  float altitude;
  int pressure;
};

const int smoke_pin = A0;
extern Adafruit_BMP085 bmp180;

unsigned int mq2Measure();
BmpVal bmpMeasure();

} // namespace sensors

#endif
