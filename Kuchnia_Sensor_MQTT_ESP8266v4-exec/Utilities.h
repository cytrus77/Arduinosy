#ifndef UTILITIES_H
#define UTILITIES_H

class dimmer
{
public:
	static int m_dimmerCounter;
        static dimmer *dimmerPtr[2];
        int m_trigger;
        
private:
        int m_setValue;
        int m_timer;
	unsigned long m_startTimer;
        int m_currentValue;
	int m_pin;
	int m_mqttTopic;
        int m_mqttTrigger;
        int m_mqttTimer;

public:
        dimmer(int topic, int pin, int topicTimer, int topicTrigger);
	void setDimmer(void);
        void setTrigger(int trigger);
        void setValue(int value);
        void setTimer(int timer);
        void resetTimer();
};

dimmer *dimmer::dimmerPtr[2];
int dimmer::m_dimmerCounter = 0;

dimmer::dimmer(int topic, int pin, int topicTimer, int topicTrigger)
{
	m_currentValue = 0;
	m_setValue = 0;
        m_trigger = TRIGERVALUE;
        m_timer   = TIMERVALUE;
	m_pin = pin;
	m_mqttTopic   =  topic;
        m_mqttTrigger =  topicTrigger;
	m_mqttTimer   =  topicTimer;
	m_dimmerCounter++;
	dimmerPtr[m_dimmerCounter-1] = this;
	pinMode(m_pin, OUTPUT);
}

void dimmer::setDimmer()
{
        if( (millis() - m_startTimer)/1000 > m_timer)
        {
                m_setValue = 0;
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
         sendBuffer[getIndex][0] = m_mqttTopic;
         sendBuffer[getIndex][1] = m_setValue;
         getIndex++;
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
         sendBuffer[getIndex][0] = m_mqttTrigger;
         sendBuffer[getIndex][1] = m_trigger;
         getIndex++;
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
         sendBuffer[getIndex][0] = m_mqttTimer;
         sendBuffer[getIndex][1] = m_timer;
         getIndex++;
      }
}

void dimmer::resetTimer()
{
      m_startTimer = millis();
}


void sendUptime()
{
      static unsigned long uptime = 0;  
      int old_value = uptime;
      uptime = millis()/60000;
      if(old_value != uptime)
      {
         sendBuffer[getIndex][0] = MQTT_UPTIME;
         sendBuffer[getIndex][1] = uptime;
         getIndex++;
      }
      #ifdef DEBUG
      Serial.println("sendUptime()");
      #endif
}

#endif
