#ifndef UTILITIES_H
#define UTILITIES_H

#include <mqtt.h>

#define TRIGERVALUE 50  // default value for photo trigger

extern MQTT mqtt;//(&esp);

class dimmer
{
public:
        int m_setValue;
        int m_trigger;

private:
	int m_currentValue;
	int m_pin;
	int m_mqttTopic;
        int m_mqttTrigger;

public:
        dimmer(int topic, int pin, int topicTrigger);
	void setDimmer(void);
        void setTrigger(int trigger);
        void setValue(int value);
};

dimmer::dimmer(int topic, int pin, int topicTrigger)
{
	char topicChar[5];
	m_currentValue = 0;
	m_setValue = 0;
        m_trigger = TRIGERVALUE;
	m_pin = pin;
	m_mqttTopic = topic;
        m_mqttTrigger = topicTrigger;
	
	pinMode(m_pin, OUTPUT);
	itoa(m_mqttTopic, topicChar, 10);
	mqtt.subscribe(topicChar); 
}

void dimmer::setDimmer()
{
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
      if(trigger > 0 && trigger < 100) m_trigger = trigger;
      else if (trigger < 0)            m_trigger = 0;
      else if (trigger > 100)          m_trigger = 100;
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
