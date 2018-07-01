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
    m_mqttClient(0),
  	m_espMqttClient(0),
  	m_interfaceType(0)
{}

uptime::uptime(const String& topic, PubSubClient* client)
  : m_mqttTopic(topic),
    m_uptime(0),
    m_sendFlag(true),
    m_mqttClient(client),
  	m_espMqttClient(0),
  	m_interfaceType(EEthernetShield)
{}

uptime::uptime(const String& topic, MQTT* client)
  : m_mqttTopic(topic),
    m_uptime(0),
    m_sendFlag(true),
    m_mqttClient(0),
    m_espMqttClient(client),
  	m_interfaceType(EESP8266)
{}

void uptime::getUptime()
{
  int old_value = m_uptime;
  m_uptime = (int)(millis()/60000);

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

  	if (m_interfaceType == EEthernetShield)
  	{
      m_mqttClient->publish(m_mqttTopic.c_str(), dataChar);
  	}
  	else if (m_interfaceType == EESP8266)
  	{
      #ifndef ESP8266
  //    m_espMqttClient->publish(topicChar, dataChar);
      #endif
  	}

    m_sendFlag = false;
    #ifdef DEBUG
    Serial.print("uptime::sendIfChanged - sending. topic=");
    Serial.print(m_mqttTopic.c_str());
    Serial.print(" value=");
    Serial.println(m_uptime);
    #endif
  }
}
