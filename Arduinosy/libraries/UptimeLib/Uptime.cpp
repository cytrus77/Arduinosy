#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEBUG 1

#include "Uptime.h"
#include <PubSubClient.h>

// extern PubSubClient client;
// extern
uptime::uptime(const String& topic)
  : m_mqttTopic(topic),
    m_uptime(0),
    m_sendFlag(true),
    m_mqttClient(0)
{}

uptime::uptime(const String& topic, PubSubClient* client)
  : m_mqttTopic(topic),
    m_uptime(0),
    m_sendFlag(true),
    m_mqttClient(client),
{}

void uptime::getUptime()
{
  long old_value = m_uptime;
  m_uptime = (long)(millis()/60000);

  if(old_value != m_uptime)
  {
    m_sendFlag = true;
    #ifdef DEBUG
    Serial.println("uptime::getUptime() -> m_sendFlag = 1");
    #endif
  }
}

void uptime::sendIfChanged()
{
  if (m_sendFlag)
  {
    char dataChar[10];
    itoa(m_uptime, dataChar, 10);
    m_espMqttClient->publish(topicChar, dataChar);
    m_sendFlag = false;
    #ifdef DEBUG
    Serial.print("uptime::sendIfChanged - sending. topic=");
    Serial.print(m_mqttTopic.c_str());
    Serial.print(" value=");
    Serial.println(m_uptime);
    #endif
  }
}
