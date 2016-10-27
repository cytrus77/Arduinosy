#ifndef STATUSLED_H
#define STATUSLED_H

#define LED_ON   false
#define LED_OFF  true

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
  unsigned long m_timer_duty_cycle;

public:
  statusled(int pin, led_mode mode, unsigned long timer_duty_cycle);
  void turnOn();
  void turnOff();
  void setMode(led_mode mode);
  bool getState();
  void processTimer();
};

#endif

