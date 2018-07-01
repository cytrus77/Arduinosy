#ifndef Dimmer_H
#define Dimmer_H

#include <string.h>


class Dimmer
{
private:
  int m_setValue;
  String m_mqttTopic;
  String m_mqttTimeoutTopic;
  unsigned long m_cyclesInSecond;
  unsigned long m_timeout;
  unsigned long m_timer;
  int m_currentValue;
  int m_pin;

  void setupHw();

public:
  Dimmer(const int pin, const String& mqttTopic);
  Dimmer(const int pin, const String& mqttTopic, const String& mqttTimeoutTopic,
         unsigned long timeout, unsigned long cyclesInSecond);
  void setDimmer(void);
  void setValue(int value);
  void setTimeout(unsigned long timeout);

  const String& getMqttTopic();
  const String& getTimeoutMqttTopic();
  int getValue();
  int getCurrentValue();

  void processTimer(const unsigned int newValue = 0);
  void resetTimer();
};

#endif
