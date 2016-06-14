#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Roller.h"

roller::roller(int topic, int pin_up, int pin_down, unsigned long timeout_set)
{
  m_state = OFF;
  m_topic = topic;
  m_pin_up   = pin_up;
  m_pin_down = pin_down;
  m_roller_timeout = timeout_set;
  pinMode(m_pin_up, OUTPUT);
  pinMode(m_pin_down, OUTPUT);
  digitalWrite(m_pin_up, m_state);
  digitalWrite(m_pin_down, m_state);
}

void roller::up()
{
  m_timeout_timer = m_roller_timeout;
  
  if (!((m_direction == UP) && (m_state == ON)))
  {
    m_state = ON;
    m_direction = UP;
    digitalWrite(m_pin_up, m_state);
    digitalWrite(m_pin_down, OFF);
    Serial.println("Up");
  }
}

void roller::down()
{
  m_timeout_timer = m_roller_timeout;

  if (!((m_direction == DOWN) && (m_state == ON)))
  {
    m_state = ON;
    m_direction = DOWN;
    digitalWrite(m_pin_up, OFF);
    digitalWrite(m_pin_down, m_state);
    Serial.println("Down");
  }
}

void roller::stop()
{
  if (m_state == ON)
  {
    m_state = OFF;
    digitalWrite(m_pin_up, m_state);
    digitalWrite(m_pin_down, m_state);
    Serial.println("STOP");
  }
}

void roller::setState(bool state, bool direction)
{
  if (state == ON)
  {
    if (direction == UP)
    {
      up();
    }
    else if (direction == DOWN)
    {
      down();
    }
  }
  else
  {
    stop();
  }

  Serial.print("state => ");
  Serial.println(m_state);
  Serial.print("direction => ");
  Serial.println(m_direction);
}

int roller::getTopic()
{
  return m_topic;
}

bool roller::getState()
{
  return m_state;
}

void roller::checkTimeout()
{
  if (m_state == ON)
  {   
    --m_timeout_timer;

    if (m_timeout_timer <= 0)
    {
      stop();
    }
  }
}


