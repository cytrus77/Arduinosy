#ifndef CZUJNIK_H
#define CZUJNIK_H

class sensor
{
public:
	static int    m_sensorCounter;
	static sensor *sensorPtr[6];
	int           m_value;

private:
	int  m_mqttTopic;
	int  m_pin;
	bool m_invertedScale;
	bool m_inputType;
        unsigned long m_timer;

public:
	sensor(int topic, int pin, bool invertedScale, bool inputType);
	void doMeasure();
};

#endif

