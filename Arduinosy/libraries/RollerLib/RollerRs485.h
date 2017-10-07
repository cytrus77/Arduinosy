#ifndef ROLLER_H
#define ROLLER_H

#include <string.h>


class RollerRs485
{
public:
  enum EState : unsigned int
  {
    Off = 0,
    Up,
    Down
  };

  const String stopCmd = "Stop";
  const String upCmd   = "Up";
  const String downCmd = "Down";
  const String ackCmd  = "Ack";

private:
  const String m_topic;
  EState m_state;
  const unsigned long m_roller_timeout;  // in timer duty cycle
  unsigned long m_timeout_timer;   // in timer duty cycle

public:
  RollerRs485(const String& topic, unsigned long timeout_set);
  void up();
  void down();
  void stop();
  const String&  getMqttTopic();
  EState getState();
  void tick();
};

#endif
