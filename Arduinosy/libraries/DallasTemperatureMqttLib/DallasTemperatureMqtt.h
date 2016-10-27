#ifndef DALLASTEMPERATURE_H
#define DALLASTEMPERATURE_H

#include "IMqttSensor.h"
#include "DallasTemperature.h"

#define ETHERNET 1

#ifdef ETHERNET
#include <PubSubClient.h>
#elif ESP8266
#include <espduino.h>
typedef MQTT PubSubClient;
#endif

class DallasTemperatureMqtt : public IMqttSensor, public DallasTemperature
{
private:
	float m_value;

public:
  DallasTemperatureMqtt(int topic, PubSubClient* client, OneWire* oneWire, unsigned long send_period);
  virtual ~DallasTemperatureMqtt();
  
  void doMeasure();
  bool doMeasureIfItsTime();
  bool doMeasureAndSendDataIfItsTime();
  bool doMeasureAndSendDataIfItsTimeAndValueChanged();

  bool sendData();
  void sendMqttPacket(int topic, float value);
};

#endif





