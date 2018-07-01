#include "sensors.hpp"

namespace sensors
{

OneWire oneWire(ds18b20_pin);
DallasTemperature ds18b20(&oneWire);
DHT dht11(dht11_pin, DHT11);
DHT dht22(dht22_pin, DHT22);


unsigned int mq2Measure()
{
  return analogRead(smoke_pin) * 100 / 1024;
}

float ds18b20Measure()
{
  float temp = -100;
  unsigned int tryCounter = 0;
  do {
    ds18b20.requestTemperatures();
    temp = ds18b20.getTempCByIndex(0);
    ++tryCounter;
  } while ((temp == 85.0 || temp == (-127.0)) && tryCounter < 10);

  return temp;
}

DhtVal dhtMeasure(DHT& dht)
{
  DhtVal val;
  val.temp = dht.readTemperature();
  val.humid = dht.readHumidity();
  return val;
}

DhtVal dht11Measure()
{
  return dhtMeasure(dht11);
}

DhtVal dht22Measure()
{
  return dhtMeasure(dht22);
}

} // namespace sensors
