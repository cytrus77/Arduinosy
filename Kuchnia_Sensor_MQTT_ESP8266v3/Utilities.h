#ifndef UTILITIES_H
#define UTILITIES_H

#include <mqtt.h>
extern MQTT mqtt;//(&esp);

class dimmer
{
private:
	int m_currentValue;
	int m_setValue;
	int m_pin;
	int m_mqttTopic;

public:
        dimmer(int topic, int pin);
	void setDimmer(int valueToSet);
};

dimmer::dimmer(int topic, int pin)
{
	char topicChar[5];
	
	m_currentValue = 0;
	m_setValue = 0;
	m_pin = pin;
	m_mqttTopic = topic;
	
	pinMode(m_pin, OUTPUT);
	itoa(m_mqttTopic, topicChar, 10);
	mqtt.subscribe(topicChar);
}

void dimmer::setDimmer(int valueToSet)
{
	if(m_currentValue != valueToSet)
	{
		if(m_currentValue < valueToSet) m_currentValue++;
		else if(m_currentValue > valueToSet) m_currentValue--;
	}
	analogWrite(m_pin, m_currentValue*m_currentValue/40);
}

void getUptime(unsigned long * uptime)
{
      *uptime = millis()/60000;
}

#endif
