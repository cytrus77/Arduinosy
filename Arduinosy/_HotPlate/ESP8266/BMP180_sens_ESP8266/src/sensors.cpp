#include "sensors.hpp"


namespace sensors
{

Adafruit_BMP085 bmp180;


unsigned int mq2Measure()
{
  return analogRead(smoke_pin) * 100 / 1024;
}

BmpVal bmpMeasure()
{
  BmpVal val;
  val.temp = bmp180.readTemperature();  // C degree
  val.pressure = bmp180.readPressure(); // Pascals
  val.altitude = bmp180.readAltitude(101500); // meters
  return val;
}

} // namespace sensors
