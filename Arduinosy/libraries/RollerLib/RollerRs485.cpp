#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "RollerRs485.h"

RollerRs485::RollerRs485(const String& topic, unsigned long timeout_set)
  : m_topic(topic)
  , m_roller_timeout(timeout_set)
{
  m_state = EState::Off;
}

void RollerRs485::up()
{
  m_timeout_timer = m_roller_timeout;
  m_state = EState::Up;
  Serial.println(upCmd);
}

void RollerRs485::down()
{
  m_timeout_timer = m_roller_timeout;
  m_state = EState::Down;
  Serial.println(downCmd);
}

void RollerRs485::stop()
{
  m_state = EState::Off;
  Serial.println(stopCmd);
}

const String& RollerRs485::getMqttTopic()
{
  return m_topic;
}

RollerRs485::EState RollerRs485::getState()
{
  return m_state;
}

void RollerRs485::tick()
{
  if (m_state != EState::Off)
  {
    --m_timeout_timer;

    if (m_timeout_timer <= 0)
    {
      stop();
    }
  }
}
