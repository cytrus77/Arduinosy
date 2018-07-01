#ifndef UPTIME_H
#define UPTIME_H

#include <PubSubClient.h>
#include <string.h>

class MQTT;

class uptime
{
private:
  String m_mqttTopic;
  long m_uptime;
  bool m_sendFlag;
  PubSubClient* m_mqttClient;

public:
  uptime(const String& topic);
  uptime(const String& topic, PubSubClient* client);
  uptime(const String& topic, MQTT* client);

  void getUptime(void);
  void sendIfChanged();
};

#endif
