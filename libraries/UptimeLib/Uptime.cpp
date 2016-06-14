#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Uptime.h"
#include <PubSubClient.h>

extern PubSubClient client;

uptime::uptime(int topic)
{
	m_mqttTopic  = topic;
	m_uptime     = 0;
	m_sendFlag   = true;
}

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
    char topicChar[6];
    char dataChar[6];
    itoa(m_mqttTopic, topicChar, 10);
    itoa(m_uptime, dataChar, 10);
    client.publish(topicChar, dataChar);
    Serial.println("uptime - wysylanie");
    m_sendFlag = false;
  }
}

