#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"
#include "Defines.h"

dimmer::dimmer(int topic, int pin)
{
	m_currentValue = 0;
	m_setValue     = 0;
	m_pin          = pin;
	m_mqttTopic    =  topic;
  m_sendFlagValue     = 0;
	pinMode(m_pin, OUTPUT);
}

void dimmer::setDimmer()
{
	if( (millis() - m_startTimer)/1000 > m_timer)
	{
		setValue(0);
	}

	if(m_currentValue != m_setValue)
	{
		if		(m_currentValue < m_setValue) ++m_currentValue;
		else if	(m_currentValue > m_setValue) --m_currentValue;
				analogWrite(m_pin, m_currentValue*m_currentValue/255);
	}
}

void dimmer::setValue(int value)
{
	int old_value = m_setValue;
	if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
	else if (value < 0)            m_setValue = 0;
	else if (value > 100)          m_setValue = 255;
	resetTimer();
	if(old_value != m_setValue)
	{
		m_sendFlagValue = 1;
	}
}

void dimmer::resetTimer()
{
	m_startTimer = millis();
}

