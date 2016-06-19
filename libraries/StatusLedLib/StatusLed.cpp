#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "StatusLed.h"

statusled::statusled(int pin, led_mode mode, unsigned long timer_duty_cycle)
{
  m_state  = LED_OFF;
  m_mode   = mode;
  m_pin    = pin;
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, m_state);
  m_timer  = 0;
  m_period = 0;
  m_timer_duty_cycle = timer_duty_cycle;
}

void statusled::turnOn()
{
  if (m_state != LED_ON)
  {
    m_state = LED_ON;
    digitalWrite(m_pin, m_state);
  }
}
  
void statusled::turnOff()
{
  if (m_state != LED_OFF)
  {
    m_state = LED_OFF;
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
      case poweron:
      m_period = 0xFFFFFFFF;
      break;
    
      case offline:
      m_period = 2 * 1000000 / m_timer_duty_cycle;
      break;
    
      case online:
      m_period = 5 * 100000 / m_timer_duty_cycle;
      break;
    
      default:
      break;
    }

    m_timer = m_period;
  }
}

bool statusled::getState()
{
  return m_state;
}

void statusled::checkTimer()
{ 
  if (m_mode == off)
  {
    turnOff();
  }
  else if (m_mode == poweron)
  {
    turnOn();
  }
  else if(m_timer == 0)
  {
    if (m_state == LED_ON)
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
