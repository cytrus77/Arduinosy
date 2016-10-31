#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"

//#define DEBUG

dimmer::dimmer(int pin, int mqttTopic)
  : m_setValue(0),
    m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(0),
    m_timeout(0),
    m_timer(0),
    m_currentValue(0),
    m_pin(pin)
{
  pinMode(m_pin, OUTPUT);
  analogWrite(m_pin, m_setValue);
}

dimmer::dimmer(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout)
  : m_setValue(0),
    m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(mqttTimeoutTopic),
    m_timeout(timeout),
    m_timer(timeout),
    m_currentValue(0),
    m_pin(pin)
{
  pinMode(m_pin, OUTPUT);
  analogWrite(m_pin, m_setValue);
}

void dimmer::setDimmer()
{
  if(m_currentValue != m_setValue)
  {
    if (m_currentValue < m_setValue)      ++m_currentValue;
    else if (m_currentValue > m_setValue) --m_currentValue;

    analogWrite(m_pin, m_currentValue*m_currentValue/255);

    #ifdef DEBUG
    Serial.print("Dimmer value=");
    Serial.println(m_currentValue);
    #endif
  }
}

void dimmer::setValue(int value)
{
  if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
  else if (value < 0)            m_setValue = 0;
  else if (value > 100)          m_setValue = 255;

  resetTimer();

  #ifdef DEBUG
  Serial.print("Dimmer value changed =");
  Serial.println(m_setValue);
  #endif
}

void dimmer::setTimeout(unsigned long timeout)
{
  if(timeout >= 0)
  {
    m_timeout = timeout;
  }

  m_timer = m_timeout;
}

void dimmer::resetTimer()
{
  m_timer = m_timeout;
}

int dimmer::getMqttTopic()
{
  return m_mqttTopic;
}

int dimmer::getTimeoutMqttTopic()
{
  return m_mqttTimeoutTopic;
}

int dimmer::getValue()
{
  return m_setValue;
}

void dimmer::processTimer()
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
