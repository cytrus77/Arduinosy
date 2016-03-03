#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "StatusLed.h"
#include "Defines.h"

statusled::statusled(int pin, led_mode mode)
{
  m_state  = OFF;
  m_mode   = mode;
  m_pin    = pin;
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, m_state);
  m_timer  = 0;
  m_period = 1;
}

void statusled::turnOn()
{
  if (m_state != ON)
  {
    m_state = ON;
    digitalWrite(m_pin, m_state);
  }
}
  
void statusled::turnOff()
{
  if (m_state != OFF)
  {
    m_state = OFF;
    digitalWrite(m_pin, m_state);
  }
}

void statusled::setMode(led_mode mode)
{
  if (m_mode != mode)
  {
    m_mode = mode;

    switch (m_mode)
    {
      case off:
      m_period = 1;
      break;
    
      case offline:
      m_period = 2 * 1000000 / INT_TIMER_PERIOD;
      break;
    
      case online:
      m_period = 5 * 100000 / INT_TIMER_PERIOD;
      break;
    
      default:
      break;
    }
  }
}

bool statusled::getState()
{
  return m_state;
}

void statusled::checkTimer()
{ 
  if(m_timer == 0)
  {
    if (m_state == ON || m_mode == off)
    {
      turnOff();
    }
    else
    {
      turnOn();
    }

    m_timer = m_period;
  }

  --m_timer;
}
