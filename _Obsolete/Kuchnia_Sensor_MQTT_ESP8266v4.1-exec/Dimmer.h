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
	int m_mqttTrigger;
	int m_mqttTimer;
	int m_mqttTopicStatus;
	int m_mqttTriggerStatus;
	int m_mqttTimerStatus;

public:
	dimmer(int topic, int pin, int topicTimer, int topicTrigger, int topicSt, int topicTimerSt, int topicTriggerSt);
	void setDimmer(void);
	void setTrigger(int trigger);
	void setValue(int value);
	void setTimer(int timer);
	void resetTimer();
};

#endif

