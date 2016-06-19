#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"

//#define DEBUG

dimmer::dimmer(int pin, int mqttTopic)
{
  m_currentValue = 0;
  m_setValue     = 0;
  m_mqttTopic    = mqttTopic;
  m_timeout      = 60;
  m_pin          = pin;
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

void dimmer::setTimeout(int timeout)
{
  if(timeout >= 0 && timeout <= 100)
  { 
    m_timeout = timeout;
    return;
  }
  
  if (timeout < 0)         
  {
    m_timeout = 0;
  }
  else if (timeout > 100)     
  {
    m_timeout = 100;
  }
}

void dimmer::checkTimeout()
{
  if( (millis() - m_startTimer)/1000 > m_timeout)
  {
    setValue(0);
  }
}

void dimmer::setCurrentLight(int light)
{
  m_currentLight = light;
}

void dimmer::resetTimer()
{
	m_startTimer = millis();
}

int dimmer::getMqttTopic()
{
  return m_mqttTopic;
}