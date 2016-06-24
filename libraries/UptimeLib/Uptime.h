#ifndef UPTIME_H
#define UPTIME_H

#define ETHERNET 1

#ifdef ETHERNET
#include <PubSubClient.h>
#elif ESP8266
#include <espduino.h>
typedef MQTT PubSubClient;
#endif

class uptime
{
private:
  int m_mqttTopic;
  int m_uptime;
  bool m_sendFlag;
  PubSubClient* m_mqttClient;

public:
  uptime(int topic, PubSubClient* client);
  void getUptime(void);
  void sendIfChanged();
};

#endif

