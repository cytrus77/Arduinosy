#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Relay.h"

//#define DEBUG

relay::relay(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout)
  : m_setValue(LOW),
    m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(mqttTimeoutTopic),
    m_timeout(timeout),
    m_timer(timeout),
    m_pin(pin)
{
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, m_setValue);
}

void relay::setValue(int value)
{
  m_setValue = value;
  digitalWrite(m_pin, m_setValue);

  resetTimer();
  
  #ifdef DEBUG
  Serial.print("Dimmer value changed =");
  Serial.println(m_setValue);
  #endif
}

void relay::setTimeout(unsigned long timeout)
{
  if(timeout >= 0)
  { 
    m_timeout = timeout;
  }

  m_timer = m_timeout;
}

void relay::resetTimer()
{
  m_timer = m_timeout;
}

int relay::getMqttTopic()
{
  return m_mqttTopic;
}

int relay::getTimeoutMqttTopic()
{
  return m_mqttTimeoutTopic;
}

int relay::getValue()
{
  return m_setValue;
}

void relay::processTimer()
{
  if (m_timeout)
  {
	if (m_timer)
	{
	  --m_timer;
	}
	else
	{
	  setValue(0);
	}
  }
}
