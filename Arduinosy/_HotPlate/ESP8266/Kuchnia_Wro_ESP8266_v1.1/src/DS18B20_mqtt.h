#ifndef DS18B20MQTT_H
#define DS18B20MQTT_H

#include "IMqttSensor.h"
#include <DallasTemperature.h>

class ds18b20mqtt : public IMqttSensor
{
private:
  DallasTemperature* m_ds18b20;
  float m_tempValue;

  bool sendData();

public:
  ds18b20mqtt(int topic, PubSubClient* client, DallasTemperature* ds18b20, unsigned long send_period);
  virtual ~ds18b20mqtt();

  void doMeasure();
  bool doMeasureIfItsTime();
  bool doMeasureAndSendDataIfItsTime();
  bool doMeasureAndSendDataIfItsTimeAndValueChanged();
};

#endif
