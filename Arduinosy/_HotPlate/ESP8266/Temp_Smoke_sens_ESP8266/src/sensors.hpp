#ifndef SENSORS_H
#define SENSORS_H

// for temp sensors
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

namespace sensors
{

struct DhtVal
{
  float temp;
  float humid;
};

const int ds18b20_pin = 4;
const int dht11_pin = 14;
const int dht22_pin = 2;
const int smoke_pin = A0;


unsigned int mq2Measure();
float ds18b20Measure();
DhtVal dhtMeasure(DHT& dht);
DhtVal dht11Measure();
DhtVal dht22Measure();

} // namespace sensors

#endif
