#ifndef RELAY_H
#define RELAY_H

#include <string.h>

class relay
{
public:
  enum ERelayState : unsigned int
  {
    Off = 0,
    On  = 1
  };

private:
  const String& m_mqttTopic;
  const String& m_mqttTimeoutTopic;
  unsigned long m_timeout;
  const unsigned int m_pin;
  const bool m_activeState;
  ERelayState m_setValue;
  unsigned long m_timer;

public:
  relay(const unsigned int pin,
        const String& mqttTopic,
        const String& mqttTimeoutTopic,
        const unsigned long timeout,
        const bool activeState);

  void setState(ERelayState value);
  void setTimeout(unsigned long timeout);

  const String& getMqttTopic();
  const String& getTimeoutMqttTopic();
  ERelayState getState();

  void processTimer();
  void resetTimer();
};

#endif
