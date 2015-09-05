#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
public:
        int m_trigger;
        int m_setValue;
        
private:
        int m_timer;
	unsigned long m_startTimer;
        int m_currentValue;
	int m_pin;
	int m_mqttTopic;
        int m_mqttTrigger;
        int m_mqttTimer;
        int m_mqttTopicStatus;
        int m_mqttTriggerStatus;
        int m_mqttTimerStatus;

public:
        dimmer(int topic, int pin, int topicTimer, int topicTrigger, int topicSt, int topicTimerSt, int topicTriggerSt);
	void setDimmer(void);
        void setTrigger(int trigger);
        void setValue(int value);
        void setTimer(int timer);
        void resetTimer();
};

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
		if      (m_currentValue < m_setValue) ++m_currentValue;
		else if (m_currentValue > m_setValue) --m_currentValue;
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
         sendBuffer[getIndex][0] = m_mqttTopicStatus;
         sendBuffer[getIndex][1] = m_setValue;
         getIndex = (getIndex+1) & 0xF;
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
         sendBuffer[getIndex][0] = m_mqttTriggerStatus;
         sendBuffer[getIndex][1] = m_trigger;
         getIndex = (getIndex+1) & 0xF;
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
         sendBuffer[getIndex][0] = m_mqttTimerStatus;
         sendBuffer[getIndex][1] = m_timer;
         getIndex = (getIndex+1) & 0xF;
      }
}

void dimmer::resetTimer()
{
      m_startTimer = millis();
}

#endif
