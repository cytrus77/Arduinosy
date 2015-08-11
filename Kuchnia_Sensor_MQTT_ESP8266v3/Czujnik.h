#ifndef CZUJNIK_H
#define CZUJNIK_H

#include <mqtt.h>

extern MQTT mqtt;//(&esp);

class sensor
{
public:
	static int    m_sensorCounter;
        static sensor *sensorPtr[6];
        int           m_value;
        
private:	
	int     m_mqttTopic;
	int     m_pin;

public:	
        sensor(int topic, int pin);
	void    sendMqtt();
	void    getValue();
};

sensor::sensor(int topic, int pin)
{
	m_sensorCounter++;
	m_mqttTopic = topic;
	m_pin = pin;
	sensorPtr[m_sensorCounter-1] = this;
	pinMode(m_pin, INPUT);
}

void sensor::sendMqtt()
{
	char dataChar[5];
	char topicChar[5];
	
	itoa(m_value, dataChar, 10);
	itoa(m_mqttTopic, topicChar, 10);
	mqtt.publish(topicChar, dataChar);
}

void sensor::getValue()
{
    if(m_pin >= A0)
    {
        m_value = analogRead(m_pin);
	m_value = map(m_value, 0, 1024, 100, 0);
    }
    else m_value = digitalRead(m_pin);
}

#endif
