#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Dimmer.h"

dimmer::dimmer(int pin)
{
  m_currentValue = 0;
  m_setValue     = 0;
  m_trigger      = 50;
  m_timeout      = 60;
  m_pin          = pin;
  m_pir_flag     = true;
  pinMode(m_pin, OUTPUT);
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
  int old_value = m_setValue;
  if(value >= 0 && value <= 100) m_setValue = map(value, 0, 100, 0, 255);
  else if (value < 0)            m_setValue = 0;
  else if (value > 100)          m_setValue = 255;
  resetTimer();
}

void dimmer::setTrigger(int trigger)
{
  if(trigger >= 0 && trigger <= 100)
  { 
    m_trigger = trigger;
    return;
  }
  if (trigger < 0)            m_trigger = 0;
  else if (trigger > 100)     m_trigger = 100;
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

void dimmer::checkPir()
{
  resetTimer();
  
  if ((m_currentLight < m_trigger) && !m_setValue && m_pir_flag) 
  {
    setValue(50);
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

void dimmer::setPirFlag(bool flag)
{
  m_pir_flag = flag;
}

