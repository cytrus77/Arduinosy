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
        bool          m_invertedScale;
        
private:	
	int     m_mqttTopic;
	int     m_pin;

public:	
        sensor(int topic, int pin, bool invertedScale);
	void    sendMqtt();
	void    getValue();
};

sensor::sensor(int topic, int pin, bool invertedScale)
{
	m_sensorCounter++;
	m_mqttTopic = topic;
	m_pin = pin;
        m_invertedScale = invertedScale;
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
        int tempValue = analogRead(m_pin);
        if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
        else                m_value = map(tempValue, 0, 1024, 0, 100);
    }
    else m_value = digitalRead(m_pin);
}

#endif
