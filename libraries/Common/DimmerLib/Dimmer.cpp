#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"


Dimmer::Dimmer(const int pin, const String& mqttTopic)
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

Dimmer::Dimmer(const int pin, const String& mqttTopic, const String& mqttTimeoutTopic,
               unsigned long timeout, unsigned long cyclesInSecond)
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

void Dimmer::setupHw()
{
  pinMode(m_pin, OUTPUT);
  #ifdef ESP8266
  analogWriteFreq(500);
  analogWriteRange(255);
  digitalWrite(m_pin, 0);
  #endif
  analogWrite(m_pin, m_setValue);
}

void Dimmer::setDimmer()
{
  if(m_currentValue != m_setValue)
  {
    if (m_currentValue < m_setValue)      ++m_currentValue;
    else if (m_currentValue > m_setValue) --m_currentValue;

    analogWrite(m_pin, m_currentValue*m_currentValue/255);
  }
}

void Dimmer::setValue(int value)
{
  if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
  else if (value < 0)            m_setValue = 0;
  else if (value > 100)          m_setValue = 255;

  resetTimer();
}

void Dimmer::setTimeout(unsigned long timeout)
{
  m_timeout = timeout;
  m_timer = m_timeout * m_cyclesInSecond;
}

void Dimmer::resetTimer()
{
  m_timer = m_timeout * m_cyclesInSecond;
}

const String& Dimmer::getMqttTopic()
{
  return m_mqttTopic;
}

const String& Dimmer::getTimeoutMqttTopic()
{
  return m_mqttTimeoutTopic;
}

int Dimmer::getValue()
{
  return m_setValue;
}

int Dimmer::getCurrentValue()
{
  return m_currentValue;
}

void Dimmer::processTimer(const unsigned int newValue)
{
  if (m_timeout)
  {
  	if (m_timer) --m_timer;
  	else setValue(newValue);
  }
}
