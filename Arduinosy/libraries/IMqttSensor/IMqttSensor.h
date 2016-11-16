#ifndef IMQTTSENSOR_H
#define IMQTTSENSOR_H

#define ETHERNET 1

#ifdef ETHERNET
#include <PubSubClient.h>
#elif ESP8266
#include <espduino.h>
typedef MQTT PubSubClient;
#endif

#include "Utils.h"

class IMqttSensor
{
protected:
  int  m_value;
  int  m_mqttTopic;
  bool m_itsTimeToSendFlag;
  bool m_valureChangedFlag;
  unsigned long m_sendPeriod;
  unsigned long m_sendTimer;
  PubSubClient* m_mqttClient;

public:
  IMqttSensor(int topic, PubSubClient* client, unsigned long send_period);
  virtual ~IMqttSensor();

  virtual void processTimer();
  virtual bool isItTimeToSend();
  virtual bool isValueChanged();
  virtual void clearSendFlag();

  virtual bool sendData();
  virtual bool sendDataIfValueChanged();
  virtual bool sendDataIfItsTime();
  virtual bool sendDataIfItsTimeAndValueChanged();
  virtual void sendMqttPacket(int topic, int value);
  virtual void sendMqttPacket(int topic, float value);

  virtual int getMqttTopic();
  virtual int getValue();
  virtual int* getValuePtr();

};

#endif
