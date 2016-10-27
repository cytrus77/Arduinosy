#ifndef MQTTSENSOR_H
#define MQTTSENSOR_H

#include "IMqttSensor.h"

enum EInputType
{
	ANALOGTYPE  = 0,
	DIGITALTYPE = 1
};

enum EScaleType
{
	NORMALSCALE   = 0,
	INVERTEDSCALE = 1
};


class mqttSensor : public IMqttSensor
{
private:
  int  m_pin;
  bool m_invertedScale;
  bool m_inputType;

public:
  mqttSensor(int topic, PubSubClient* client, int pin, EInputType inputType, EScaleType invertedScale, unsigned long send_period);
  virtual ~mqttSensor();
  
  void doMeasure();
  bool doMeasureIfItsTime();
  bool doMeasureAndSendDataIfItsTime();
  bool doMeasureAndSendDataIfItsTimeAndValueChanged();
};

#endif

