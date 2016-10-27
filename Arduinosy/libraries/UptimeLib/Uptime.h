#ifndef UPTIME_H
#define UPTIME_H

#include <PubSubClient.h>
#include <mqtt.h>

class MQTT;

class uptime
{
	enum EInterfaceType
	{
		EthernetShield = 0,
		ESP8266        = 1
	};

private:
  int m_mqttTopic;
  int m_uptime;
  bool m_sendFlag;
  PubSubClient* m_mqttClient;
  MQTT* m_espMqttClient;
  const int m_interfaceType;

public:
  uptime(int topic, PubSubClient* client);
  uptime(int topic, MQTT* client);

  void getUptime(void);
  void sendIfChanged();
};

#endif
