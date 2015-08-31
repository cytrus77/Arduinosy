#ifndef CZUJNIK_H
#define CZUJNIK_H


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
