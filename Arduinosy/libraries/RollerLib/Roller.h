#ifndef ROLLER_H
#define ROLLER_H

#define ROLLER_UP    true
#define ROLLER_DOWN  false
#define ROLLER_ON    false
#define ROLLER_OFF   true

class roller
{
public:
  enum ActiveState : unsigned int
  {
    Low = 0,
    High = 1
  };

private:
  int  m_topic;
  bool m_state;
  bool m_direction;
  int  m_pin_up;
  int  m_pin_down;
  ActiveState m_activeState;
  unsigned long m_roller_timeout;  // in timer duty cycle
  unsigned long m_timeout_timer;   // in timer duty cycle

public:
  roller(int topic, int pin_up, int pin_down, unsigned long timeout_set, ActiveState state);
  void up();
  void down();
  void stop();
  void setState(bool state, bool direction);
  int  getTopic();
  bool getState();
  void checkTimeout();
};

#endif
