#ifndef STATUSLED_H
#define STATUSLED_H

class statusled
{
public:
enum led_mode
{
  off     = 0,
  poweron = 1,
  offline = 2,
  online  = 3
};
  
private:
  bool m_state;
  int  m_mode;
  int  m_pin;
  unsigned long m_timer;
  unsigned long m_period;
	
public:
  statusled(int pin, led_mode mode);
  void turnOn();
  void turnOff();
  void setMode(led_mode mode);
  bool getState();
  void checkTimer();
};

#endif

