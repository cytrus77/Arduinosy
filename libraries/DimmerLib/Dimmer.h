#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
private:
  int m_setValue;
  int m_mqttTopic;
  int m_mqttTimeoutTopic;
  unsigned long m_timeout;
  unsigned long m_timer;
  int m_currentValue;
  int m_pin;

public:
  dimmer(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout);
  void setDimmer(void);
  void setValue(int value);
  void setTimeout(unsigned long timeout);
  
  int getMqttTopic();
  int getTimeoutMqttTopic();
  int getValue();
  
  void processTimer();
  void resetTimer();
};

#endif

