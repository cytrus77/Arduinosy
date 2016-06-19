#ifndef DIMMER_H
#define DIMMER_H

class dimmer
{
private:
  int m_setValue;
  int m_mqttTopic;
  int m_timeout;
  unsigned long m_startTimer;
  int m_currentValue;
  int m_currentLight;
  int m_pin;

public:
  dimmer(int pin, int mqttTopic);
  void setDimmer(void);
  void setValue(int value);
  void setTimeout(int timeout);
  void setCurrentLight(int light);
  
  int getMqttTopic();
  
  void checkTimeout();
  void resetTimer();
};

#endif
