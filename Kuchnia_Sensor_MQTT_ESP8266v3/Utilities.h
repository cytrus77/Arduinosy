#ifndef UTILITIES_H
#define UTILITIES_H

#include <mqtt.h>

#define TRIGERVALUE  50   // default value for photo trigger [%]
#define TIMERVALUE  120   // default value of timer [sec]

extern MQTT mqtt;//(&esp);

class dimmer
{
private:
        int m_setValue;
        int m_trigger;
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

dimmer::dimmer(int topic, int pin, int topicTimer, int topicTrigger)
{
	char topicChar[5];
	m_currentValue = 0;
	m_setValue = 0;
        m_trigger = TRIGERVALUE;
        m_timer   = TIMERVALUE;
	m_pin = pin;
	m_mqttTopic   =  topic;
        m_mqttTrigger =  topicTrigger;
	m_mqttTimer   =  topicTimer;

	pinMode(m_pin, OUTPUT);
	itoa(m_mqttTopic, topicChar, 10);
	mqtt.subscribe(topicChar);
        itoa(m_mqttTrigger, topicChar, 10);
	mqtt.subscribe(topicChar);
        itoa(m_mqttTimer, topicChar, 10);
	mqtt.subscribe(topicChar);
}

void dimmer::setDimmer()
{
        if( (millis() - m_startTimer)/1000 > m_timer)
        {
                m_setValue = 0;
        }
        
    	if(m_currentValue != m_setValue)
	{
		if(m_currentValue < m_setValue) m_currentValue++;
		else if(m_currentValue > m_setValue) m_currentValue--;
                analogWrite(m_pin, m_currentValue*m_currentValue/40);
	}
}

void dimmer::setValue(int value)
{
      if(value > 0 && value < 100) m_setValue = value;
      else if (value < 0)          m_setValue = 0;
      else if (value > 100)        m_setValue = 100;
}

void dimmer::setTrigger(int trigger)
{
      if(trigger > 0 && trigger < 18000) m_trigger = trigger;
      else if (trigger < 0)              m_trigger = 0;
      else if (trigger > 18000)          m_trigger = 18000;
}

void dimmer::setTimer(int timer)
{
      if(timer > 0 && timer < 6000)  m_timer = timer;
      else if (timer < 0)           m_timer = 0;
      else if (timer > 100)         m_timer = 100;
}

void dimmer::resetTimer()
{
      m_startTimer = millis();
}

void sendUptime()
{
      static unsigned long uptime;
      unsigned long newUptime;
      
      uptime = millis()/60000;
      if(uptime != newUptime)
      {
          char dataChar[8];
          itoa(uptime, dataChar, 10);
          mqtt.publish("1000", dataChar);
          newUptime = uptime;
      }
}

#endif
