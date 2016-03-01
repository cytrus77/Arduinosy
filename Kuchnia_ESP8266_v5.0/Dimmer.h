#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
private:
  int m_trigger;
  int m_setValue;
	int m_timeout;
	unsigned long m_startTimer;
	int m_currentValue;
  int m_currentLight;
	int m_pin;
  bool m_pir_flag;

public:
	dimmer(int pin);
	void setDimmer(void);
	void setTrigger(int trigger);
	void setValue(int value);
	void setTimeout(int timeout);
  void setCurrentLight(int light);
  void checkPir();
  void checkTimeout();
	void resetTimer();
  void setPirFlag(bool flag);
};

#endif

