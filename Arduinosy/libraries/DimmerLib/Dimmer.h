#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
private:
  int m_setValue;
  int m_mqttTopic;
  int m_mqttTimeoutTopic;
  unsigned long m_cyclesInSecond;
  unsigned long m_timeout;
  unsigned long m_timer;
  int m_currentValue;
  int m_pin;

  void setupHw();

public:
  dimmer(int pin, int mqttTopic);
  dimmer(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout, unsigned long cyclesInSecond);
  void setDimmer(void);
  void setValue(int value);
  void setTimeout(unsigned long timeout);

  int getMqttTopic();
  int getTimeoutMqttTopic();
  int getValue();
  int getCurrentValue();

  void processTimer(const unsigned int newValue = 0);
  void resetTimer();
};

#endif
