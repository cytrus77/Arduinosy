#ifndef CZUJNIK_H
#define CZUJNIK_H

#define ANALOGTYPE       0
#define DIGITALTYPE      1
#define INVERTEDSCALE    1
#define NORMALSCALE      0


class sensor
{
private:
  int  m_value;
  int  m_mqttTopic;
  bool m_sendFlag;
  unsigned long m_sendPeriod;
  unsigned long m_sendTimer;
  int  m_pin;
  bool m_invertedScale;
  bool m_inputType;

public:
  sensor(int topic, int pin, bool invertedScale, bool inputType, unsigned long send_period);
  
  void doMeasure();
  void checkTimer();
  bool isDataSend();

  int getMqttTopic();
  int getValue();
};

#endif

