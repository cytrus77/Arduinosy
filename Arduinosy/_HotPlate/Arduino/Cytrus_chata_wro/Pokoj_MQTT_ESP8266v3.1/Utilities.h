#ifndef UTILITIES_H
#define UTILITIES_H

#include <mqtt.h>

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
        int m_mqttTopicFB;
        int m_mqttTriggerFB;
        int m_mqttTimerFB;
        bool m_mqttTopicFB_flag;
        bool m_mqttTriggerFB_flag;
        bool m_mqttTimerFB_flag;

public:
        int m_trigger;
        dimmer(int topic, int pin, int topicTimer, int topicTrigger, int topicFB, int topicTimerFB, int topicTriggerFB);
	void setDimmer(void);
        void setTrigger(int trigger);
        void setValue(int value);
        void setTimer(int timer);
        void resetTimer();
        void registerSubscrib();
        void sendMqtt(int* topic, int* data);
        void mqttUpdate();
};

dimmer::dimmer(int topic, int pin, int topicTimer, int topicTrigger, int topicFB, int topicTimerFB, int topicTriggerFB)
{
	m_currentValue        = 0;
	m_setValue            = 0;
        m_trigger             = TRIGERVALUE;
        m_timer               = TIMERVALUE;
	m_pin                 = pin;
	m_mqttTopic           = topic;
        m_mqttTrigger         = topicTrigger;
	m_mqttTimer           = topicTimer;
	m_mqttTopicFB         = topicFB;
        m_mqttTriggerFB       = topicTriggerFB;
	m_mqttTimerFB         = topicTimerFB;
        m_mqttTopicFB_flag    = 0;
        m_mqttTriggerFB_flag  = 0;
        m_mqttTimerFB_flag    = 0;
        
	pinMode(m_pin, OUTPUT);
}

void dimmer::setDimmer()
{
        if( (millis()/1000 - m_startTimer/1000) > m_timer && m_setValue != 0)
        {
                m_setValue = 0;
        //        sendMqtt(&m_mqttTopicFB, &m_setValue);
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
   if(value != m_setValue)
   {
      if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
      else if (value < 0)            m_setValue = 0;
      else if (value > 100)          m_setValue = 255;
      resetTimer();
      m_mqttTopicFB_flag   = 1;
   }
}

void dimmer::setTrigger(int trigger)
{
   if(trigger != m_trigger)
   {
      if(trigger >= 0 && trigger <= 100) m_trigger = trigger;
      else if (trigger < 0)              m_trigger = 0;
      else if (trigger > 100)            m_trigger = 100;
      m_mqttTriggerFB_flag = 1;
   }
}

void dimmer::setTimer(int timer)
{
   if(timer != m_timer)
   {
      if(timer >= 0 && timer <= 6000)  m_timer = timer;
      else if (timer < 0)              m_timer = 0;
      else if (timer > 6000)           m_timer = 6000;
      m_mqttTimerFB_flag   = 1;
   }
}

void dimmer::resetTimer()
{
      m_startTimer = millis();
}

void dimmer::registerSubscrib()
{
     	char topicChar[5];
        itoa(m_mqttTopic, topicChar, 10);
        mqtt.subscribe(topicChar);
        itoa(m_mqttTrigger, topicChar, 10);
      	mqtt.subscribe(topicChar);
        itoa(m_mqttTimer, topicChar, 10);
      	mqtt.subscribe(topicChar); 
}

void dimmer::sendMqtt(int* topic, int* data)
{
	char dataChar[5];
	char topicChar[5];
	
	itoa(*data, dataChar, 10);
	itoa(*topic, topicChar, 10);
	mqtt.publish(topicChar, dataChar);
}

void dimmer::mqttUpdate()
{
        if(m_mqttTopicFB_flag && !mqttBusy) 
        {
          mqttBusy = 1;
          int tempValue = map(m_setValue, 0, 255, 0, 100);
          sendMqtt(&m_mqttTopicFB, &tempValue);
          m_mqttTopicFB_flag = 0;
          mqttBusy = 0;
        }
        else if(m_mqttTriggerFB_flag && !mqttBusy)
        {
          mqttBusy = 1;
          sendMqtt(&m_mqttTriggerFB, &m_trigger);
          m_mqttTriggerFB_flag = 0;
          mqttBusy = 0;
        }
        else if(m_mqttTimerFB_flag && !mqttBusy)
        {
          mqttBusy = 1;
          sendMqtt(&m_mqttTimerFB, &m_timer);
          m_mqttTimerFB_flag = 0;
          mqttBusy = 0;
        }
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
          mqtt.publish(MQTT_UPTIME, dataChar);
          uptime = newUptime;
          mqttBusy = 0;
      }
}

#endif
