#ifndef CZUJNIK_H
#define CZUJNIK_H

extern int sendBuffer[16][2];
extern char sendIndex;
extern char getIndex;

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
	void    doMeasure();
};

sensor *sensor::sensorPtr[6];
int sensor::m_sensorCounter = 0;

sensor::sensor(int topic, int pin, bool invertedScale)
{
	m_sensorCounter++;
	m_mqttTopic = topic;
	m_pin = pin;
        m_invertedScale = invertedScale;
	sensorPtr[m_sensorCounter-1] = this;
	pinMode(m_pin, INPUT);
}

void sensor::doMeasure()
{
    int old_value = m_value;
    
    if(m_pin >= A0)
    {
        int tempValue = analogRead(m_pin);
        if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
        else                m_value = map(tempValue, 0, 1024, 0, 100);
    }
    else m_value = digitalRead(m_pin);
    #ifdef DEBUG
    Serial.println("sensor::doMeasure()");
    #endif
    if(old_value != m_value)
    {
        #ifdef DEBUG
        Serial.print("m_mqttTopic z doMeasure() = ");
        Serial.println(m_mqttTopic);
        #endif
        sendBuffer[getIndex][0] = m_mqttTopic;
        sendBuffer[getIndex][1] = m_value;
        getIndex++;
    }
}

#endif
