#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Roller.h"

roller::roller(int topic, int pin_up, int pin_down, unsigned long timeout_set)
{
  m_state = ROLLER_OFF;
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
  
  if (!((m_direction == ROLLER_UP) && (m_state == ROLLER_ON)))
  {
    m_state = ROLLER_ON;
    m_direction = ROLLER_UP;
    digitalWrite(m_pin_up, m_state);
    digitalWrite(m_pin_down, ROLLER_OFF);
    Serial.println("Up");
  }
}

void roller::down()
{
  m_timeout_timer = m_roller_timeout;

  if (!((m_direction == ROLLER_DOWN) && (m_state == ROLLER_ON)))
  {
    m_state = ROLLER_ON;
    m_direction = ROLLER_DOWN;
    digitalWrite(m_pin_up, ROLLER_OFF);
    digitalWrite(m_pin_down, m_state);
    Serial.println("Down");
  }
}

void roller::stop()
{
  if (m_state == ROLLER_ON)
  {
    m_state = ROLLER_OFF;
    digitalWrite(m_pin_up, m_state);
    digitalWrite(m_pin_down, m_state);
    Serial.println("STOP");
  }
}

void roller::setState(bool state, bool direction)
{
  if (state == ROLLER_ON)
  {
    if (direction == ROLLER_UP)
    {
      up();
    }
    else if (direction == ROLLER_DOWN)
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
  if (m_state == ROLLER_ON)
  {   
    --m_timeout_timer;

    if (m_timeout_timer <= 0)
    {
      stop();
    }
  }
}


