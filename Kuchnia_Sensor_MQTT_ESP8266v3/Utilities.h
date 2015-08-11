#ifndef UTILITIES_H
#define UTILITIES_H

#include <mqtt.h>

#define TRIGERVALUE  50   // default value for photo trigger [%]
#define TIMERVALUE  120   // default value of timer [sec]

extern MQTT mqtt;//(&esp);
extern bool mqttBusy;

class dimmer
{
private:
        int m_setValue;
        int m_timer;
	unsigned long m_startTimer;
        int m_currentValue;
	int m_pin;
	int m_mqttTopic;
        int m_mqttTrigger;
        int m_mqttTimer;
        bool m_subscribeFlag;

public:
        int m_trigger;
        dimmer(int topic, int pin, int topicTimer, int topicTrigger);
	void setDimmer(void);
        void setTrigger(int trigger);
        void setValue(int value);
        void setTimer(int timer);
        void resetTimer();
};

dimmer::dimmer(int topic, int pin, int topicTimer, int topicTrigger)
{
        m_subscribeFlag = 1;
	m_currentValue = 0;
	m_setValue = 0;
        m_trigger = TRIGERVALUE;
        m_timer   = TIMERVALUE;
	m_pin = pin;
	m_mqttTopic   =  topic;
        m_mqttTrigger =  topicTrigger;
	m_mqttTimer   =  topicTimer;

	pinMode(m_pin, OUTPUT);
}

void dimmer::setDimmer()
{
        if(m_subscribeFlag)
        {
         	char topicChar[5];
                itoa(m_mqttTopic, topicChar, 10);
	        mqtt.subscribe(topicChar);
                itoa(m_mqttTrigger, topicChar, 10);
        	mqtt.subscribe(topicChar);
                itoa(m_mqttTimer, topicChar, 10);
        	mqtt.subscribe(topicChar); 
                m_subscribeFlag = 0;
        }

        if( (millis() - m_startTimer)/1000 > m_timer)
        {
                m_setValue = 0;
        }

    	if(m_currentValue != m_setValue)
	{
                int tempValue = map(m_setValue, 0, 100, 0, 255);
		if      (m_currentValue < tempValue) ++m_currentValue;
		else if (m_currentValue > tempValue) --m_currentValue;
                analogWrite(m_pin, m_currentValue*m_currentValue/255);
                m_currentValue = map(m_currentValue, 0, 255, 0, 100);
	}
}

void dimmer::setValue(int value)
{
      if(value >= 0 && value <= 100) m_setValue = value;
      else if (value < 0)           m_setValue = 0;
      else if (value > 100)         m_setValue = 100;
      resetTimer();
}

void dimmer::setTrigger(int trigger)
{
      if(trigger >= 0 && trigger <= 18000) m_trigger = trigger;
      else if (trigger < 0)              m_trigger = 0;
      else if (trigger > 18000)          m_trigger = 18000;
}

void dimmer::setTimer(int timer)
{
      if(timer >= 0 && timer <= 6000)  m_timer = timer;
      else if (timer < 0)            m_timer = 0;
      else if (timer > 100)          m_timer = 100;
}

void dimmer::resetTimer()
{
      m_startTimer = millis();
}

void sendUptime()
{
      static unsigned long uptime;
      unsigned long newUptime;
      
      newUptime = millis()/60000;
      if(uptime != newUptime && !mqttBusy)
      {
          mqttBusy = 1;
          char dataChar[8];
          itoa(newUptime, dataChar, 10);
          mqtt.publish("1000", dataChar);
          uptime = newUptime;
          mqttBusy = 0;
      }
}

#endif
