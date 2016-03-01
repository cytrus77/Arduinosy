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
}



