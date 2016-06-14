#ifndef CZUJNIK_H
#define CZUJNIK_H

#define ANALOGTYPE       0
#define DIGITALTYPE      1
#define INVERTEDSCALE    1
#define NORMALSCALE      0
#define SENSORSENDPERIOD 30       // period for send sensor value [sec]


class sensor
{
public:
  static int    m_sensorCounter;
  static sensor *sensorPtr[6];
  int           m_value;

private:
  int  m_mqttTopic;
  int  m_pin;
  bool m_invertedScale;
  bool m_inputType;
  int  m_sendFlag;
  unsigned long m_timer;

public:
  sensor(int topic, int pin, bool invertedScale, bool inputType);
  void doMeasure();
};

#endif

