#ifndef DHTMQTT_H
#define DHTMQTT_H

#include "IMqttSensor.h"
#include "dht.h"

#define ETHERNET 1

#ifdef ETHERNET
#include <PubSubClient.h>
#elif ESP8266
#include <espduino.h>
typedef MQTT PubSubClient;
#endif

class dhtMqtt : public IMqttSensor, public dht
{
protected:
  float m_tempValue;
  int   m_tempMqttTopic;
  int   m_pin;
  
public:
  dhtMqtt(int tempTopic, int humiTopic, PubSubClient* client, int pin, unsigned long send_period);
  virtual ~dhtMqtt();
  
  void doMeasure();
  bool doMeasureIfItsTime();
  bool doMeasureAndSendDataIfItsTime();
  bool doMeasureAndSendDataIfItsTimeAndValueChanged();

  bool sendData();
  void sendMqttPacket(int topic, float value);
};

#endif





