#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"


dimmer::dimmer(int pin, int mqttTopic)
  : m_setValue(0),
    m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(0),
    m_cyclesInSecond(0),
    m_timeout(0),
    m_timer(0),
    m_currentValue(0),
    m_pin(pin)
{
  setupHw();
}

dimmer::dimmer(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout, unsigned long cyclesInSecond)
  : m_setValue(0),
    m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(mqttTimeoutTopic),
    m_cyclesInSecond(cyclesInSecond),
    m_timeout(timeout),
    m_timer(timeout * cyclesInSecond),
    m_currentValue(0),
    m_pin(pin)
{
  setupHw();
}

void dimmer::setupHw()
{
  pinMode(m_pin, OUTPUT);
  #ifdef ESP8266
  analogWriteFreq(500);
  analogWriteRange(255);
  digitalWrite(m_pin, 0);
  #endif
  analogWrite(m_pin, m_setValue);
}

void dimmer::setDimmer()
{
  if(m_currentValue != m_setValue)
  {
    if (m_currentValue < m_setValue)      ++m_currentValue;
    else if (m_currentValue > m_setValue) --m_currentValue;

    analogWrite(m_pin, m_currentValue*m_currentValue/255);
  }
}

void dimmer::setValue(int value)
{
  if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
  else if (value < 0)            m_setValue = 0;
  else if (value > 100)          m_setValue = 255;

  resetTimer();
}

void dimmer::setTimeout(unsigned long timeout)
{
  if(timeout >= 0)
  {
    m_timeout = timeout;
  }

  m_timer = m_timeout * m_cyclesInSecond;
}

void dimmer::resetTimer()
{
  m_timer = m_timeout * m_cyclesInSecond;
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

int dimmer::getCurrentValue()
{
  return m_currentValue;
}

void dimmer::processTimer(const unsigned int newValue)
{
  if (m_timeout)
  {
  	if (m_timer)
  	{
  	  --m_timer;
  	}
  	else
  	{
  	  setValue(newValue);
  	}
  }
}
