#ifndef MQTTTEMPERATURE_H
#define MQTTTEMPERATURE_H

#define ETHERNET 1

#ifdef ETHERNET
#include <PubSubClient.h>
#elif ESP8266
#include <espduino.h>
typedef MQTT PubSubClient;
#endif

class MqttTemperature
{
  enum TemperatureSensorType
  {
	  unknown = 0,
	  dht11   = 1,
	  dht22   = 2,
	  ds18b20 = 3
  };

protected:
  int  m_value;
  int  m_mqttTopic;
  bool m_itsTimeToSnedFlag;
  bool m_valureChangedFlag;
  unsigned long m_sendPeriod;
  unsigned long m_sendTimer;
  PubSubClient* m_mqttClient;
  
public:
  MqttTemperature(int tempTopic, PubSubClient* client, DallasTemperature* ds18b20, unsigned long send_period);
  MqttTemperature(int tempTopic, int humiTopic, PubSubClient* client, dht* dht, unsigned long send_period);
  virtual ~MqttTemperature();
  
  virtual void processTimer();
  virtual bool isItTimeToSend();
  virtual bool isValueChanged();
  virtual void clearSendFlag();

  virtual bool sendData();
  virtual bool sendDataIfValueChanged();
  virtual bool sendDataIfItsTime();
  virtual bool sendDataIfItsTimeAndValueChanged();
  virtual void sendMqttPacket(int topic, int value);
  
  virtual int getMqttTopic();
  virtual int getValue();
  virtual int* getValuePtr();

};

#endif





