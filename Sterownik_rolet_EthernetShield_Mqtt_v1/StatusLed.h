#ifndef STATUSLED_H
#define STATUSLED_H

class statusled
{
enum led_mode
{
  off     = 0,
  offline = 1,
  online  = 2
};
  
private:
  bool m_state;
  int  m_mode;
  int  m_pin;
	
public:
	statusled(int pin, led_mode mode);
  void turnOff();
	void setState(bool state, bool direction);
  int  getTopic();
	bool getState();
  void checkTimeout();
};

#endif

