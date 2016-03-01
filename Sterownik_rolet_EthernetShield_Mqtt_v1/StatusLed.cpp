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
  m_mode   = off;
  m_pin    = pin;
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, m_state);
  m_timer = 0;
}

void turnOn()
{
  if (m_state != ON)
  {
    m_state = ON;
    digitalWrite(m_pin, m_state);
  }
}
  
void turnOff()
{
  if (m_state != OFF)
  {
    m_state = OFF;
    digitalWrite(m_pin, m_state);
  }
}

void setMode(led_mode mode)
{
  if (m_mode != mode)
  {
    m_mode = mode;
  }
}

bool getState()
{
  return m_state;
}

void checkTimer()
{
  switch (m_mode)
  {
    case off:
    
    break;
    
    case offline:
    
    break;
    
    case online:
    
    break;
    
    default:
    break;
  }
}
