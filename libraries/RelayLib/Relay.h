#ifndef RELAY_H
#define RELAY_H

class relay
{
private:
  bool m_setValue;
  int m_mqttTopic;
  int m_mqttTimeoutTopic;
  unsigned long m_timeout;
  unsigned long m_timer;
  int m_pin;

public:
  relay(int pin, int mqttTopic, int mqttTimeoutTopic, unsigned long timeout);
  void setValue(int value);
  void setTimeout(unsigned long timeout);
  
  int getMqttTopic();
  int getTimeoutMqttTopic();
  int getValue();
  
  void processTimer();
  void resetTimer();
};

#endif

