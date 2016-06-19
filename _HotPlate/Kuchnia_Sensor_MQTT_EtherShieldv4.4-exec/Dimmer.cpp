#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"
#include "Kuchnia-defines.h"
#include "Interface.h"

dimmer::dimmer(int topic, int pin, int topicTimer, int topicTrigger, int topicSt, int topicTimerSt, int topicTriggerSt)
{
	m_currentValue = 0;
	m_setValue     = 0;
	m_trigger      = TRIGERVALUE;
	m_timer        = TIMERVALUE;
	m_pin          = pin;
	m_mqttTopic    =  topic;
	m_mqttTrigger  =  topicTrigger;
	m_mqttTimer    =  topicTimer;
	m_mqttTopicStatus   =  topicSt;
	m_mqttTriggerStatus =  topicTriggerSt;
	m_mqttTimerStatus   =  topicTimerSt;
        m_sendFlagValue     = 0;
        m_sendFlagTrigger   = 0;
        m_sendFlagTimer     = 0;
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

void dimmer::setTrigger(int trigger)
{
	int old_value = m_trigger;
	if(trigger >= 0 && trigger <= 18000) m_trigger = trigger;
	else if (trigger < 0)              m_trigger = 0;
	else if (trigger > 18000)          m_trigger = 18000;
	if(old_value != m_trigger)
	{
		m_sendFlagTrigger = 1;
	}
}

void dimmer::setTimer(int timer)
{
	int old_value = m_timer;
	if(timer >= 0 && timer <= 6000)  m_timer = timer;
	else if (timer < 0)            m_timer = 0;
	else if (timer > 100)          m_timer = 100;
	if(old_value != m_timer)
	{
		m_sendFlagTimer = 1;
	}
}

void dimmer::resetTimer()
{
	m_startTimer = millis();
}

