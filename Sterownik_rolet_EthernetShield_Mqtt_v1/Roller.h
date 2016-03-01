#ifndef ROLLER_H
#define ROLLER_H

class roller
{
private:
  int  m_topic;
  bool m_state;
  bool m_direction;
  int  m_pin_up;
  int  m_pin_down;
  unsigned long m_timeout_timer; // in ms
	
public:
	roller(int topic, int pin_up, int pin_down);
	void up();
  void down();
  void stop();
	void setState(bool state, bool direction);
  int  getTopic();
	bool getState();
  void checkTimeout();
};

#endif

