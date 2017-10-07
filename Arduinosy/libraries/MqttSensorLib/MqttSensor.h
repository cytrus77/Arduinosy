#ifndef MqttSensor_H
#define MqttSensor_H

#include "IMqttSensor.h"
#include <string.h>

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


class MqttSensor : public IMqttSensor
{
private:
  int  m_pin;
  EScaleType m_invertedScale;
  EInputType m_inputType;

public:
	MqttSensor(const String& topic, PubSubClient* client, const int pin,
		         EInputType inputType, EScaleType invertedScale, unsigned long send_period);
  virtual ~MqttSensor();

  void doMeasure();
  bool doMeasureIfItsTime();
  bool doMeasureAndSendDataIfItsTime();
  bool doMeasureAndSendDataIfItsTimeAndValueChanged();
	EInputType getInputType();
	EScaleType getScaleType();
};

#endif
