#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Uptime.h"
#include "Kuchnia-defines.h"
#include "Interface.h"


uptime::uptime(int topic)
{
	m_mqttTopic  = topic;
	m_uptime     = 0;
	m_sendFlag   = 0;
	registerToMqttTable(&m_mqttTopic,   &m_uptime, &m_sendFlag);
}

void uptime::getUptime()
{
	int old_value = m_uptime;
	m_uptime = millis()/60000;
	if(old_value != m_uptime)
	{
		m_sendFlag = 1;
		#ifdef DEBUG
		Serial.println("uptime::getUptime() -> m_sendFlag = 1");
		#endif
	}
}
