#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Relay.h"

#define DEBUG

relay::relay(const unsigned int pin,
             const String& mqttTopic,
             const String& mqttTimeoutTopic,
             const unsigned long timeout,
             const bool activeState)
  : m_mqttTopic(mqttTopic),
    m_mqttTimeoutTopic(mqttTimeoutTopic),
    m_timeout(timeout),
    m_pin(pin),
    m_activeState(activeState),
    m_setValue(ERelayState::Off),
    m_timer(timeout)
{
  pinMode(m_pin, OUTPUT);
  setState(ERelayState::Off);
}

void relay::setState(ERelayState value)
{
  m_setValue = ERelayState::On;
  digitalWrite(m_pin, (m_setValue == ERelayState::On)
                      ? m_activeState : !m_activeState);

  resetTimer();

  #ifdef DEBUG
  Serial.print("Relay value changed =");
  Serial.println(m_setValue);
  #endif
}

void relay::setTimeout(unsigned long timeout)
{
  if(timeout >= 0)
  {
    m_timeout = timeout;
  }

  m_timer = m_timeout;
}

void relay::resetTimer()
{
  m_timer = m_timeout;
}

const String& relay::getMqttTopic()
{
  return m_mqttTopic;
}

const String& relay::getTimeoutMqttTopic()
{
  return m_mqttTimeoutTopic;
}

relay::ERelayState relay::getState()
{
  return m_setValue;
}

void relay::processTimer()
{
  if (m_timeout)
  {
	  if (m_timer)
	  {
	    --m_timer;
	  }
  	else
	  {
	    setState(ERelayState::Off);
	  }
  }
}
