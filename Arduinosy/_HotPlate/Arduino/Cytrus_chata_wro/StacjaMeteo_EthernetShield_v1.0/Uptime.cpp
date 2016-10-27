#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Uptime.h"

uptime::uptime(int topic)
{
	m_mqttTopic  = topic;
	m_uptime     = 0;
}

void uptime::getUptime()
{
	m_uptime = millis()/60000;
}

