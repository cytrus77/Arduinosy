#ifndef UPTIME_H
#define UPTIME_H

#include <PubSubClient.h>
#include <string.h>

class MQTT;

class uptime
{
	enum EInterfaceType
	{
		EEthernetShield = 0,
		EESP8266        = 1
	};

private:
  String m_mqttTopic;
  int m_uptime;
  bool m_sendFlag;
  PubSubClient* m_mqttClient;
  MQTT* m_espMqttClient;
  const int m_interfaceType;

public:
  uptime(const String& topic);
  uptime(const String& topic, PubSubClient* client);
  uptime(const String& topic, MQTT* client);

  void getUptime(void);
  void sendIfChanged();
};

#endif
