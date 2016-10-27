#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
public:
	int m_trigger;
	int m_setValue;
	
private:
	int m_timer;
	unsigned long m_startTimer;
	int m_currentValue;
	int m_pin;
	int m_mqttTopic;
  int m_sendFlagValue;

public:
	dimmer(int topic, int pin);
	void setDimmer(void);
	void setTrigger(int trigger);
	void setValue(int value);
	void setTimer(int timer);
	void resetTimer();
};

#endif

