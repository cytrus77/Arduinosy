#ifndef CZUJNIK_H
#define CZUJNIK_H

#include "Interface.h"
#include "Kuchnia-objects.h"

extern int sendBuffer[64][2];
extern int sendIndex;
extern int getIndex;

class sensor
{
public:
	static int    m_sensorCounter;
        static sensor *sensorPtr[6];
        int           m_value;

        
private:	
	int     m_mqttTopic;
	int     m_pin;
        bool    m_invertedScale;
        bool    m_inputType;

public:	
        sensor(int topic, int pin, bool invertedScale, bool inputType);
	void    doMeasure();
};

sensor *sensor::sensorPtr[6];
int sensor::m_sensorCounter = 0;

sensor::sensor(int topic, int pin, bool invertedScale, bool inputType)
{
	m_sensorCounter++;
	m_mqttTopic = topic;
	m_pin = pin;
        m_inputType = inputType;
        m_invertedScale = invertedScale;
	sensorPtr[m_sensorCounter-1] = this;
	pinMode(m_pin, INPUT);
}

void sensor::doMeasure()
{
    int old_value = m_value;
    
    if(m_inputType == ANALOGTYPE)
    {
        int tempValue = analogRead(m_pin);
        if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
        else                m_value = map(tempValue, 0, 1024, 0, 100);
    }
    else if (m_inputType == DIGITALTYPE) m_value = digitalRead(m_pin);
    #ifdef DEBUG
//    Serial.println("sensor::doMeasure()");
    #endif
    if(((abs(old_value - m_value) > 1 && m_inputType == ANALOGTYPE) || m_inputType == DIGITALTYPE) && old_value != m_value)
    {
        addToSendBuffer(m_mqttTopic, m_value);
    }
}

#endif

