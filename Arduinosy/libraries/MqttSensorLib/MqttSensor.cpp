#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "MqttSensor.h"

MqttSensor::MqttSensor(const String& topic, PubSubClient* client, const int pin,
                       EInputType inputType, EScaleType invertedScale, unsigned long send_period)
 : IMqttSensor(topic, client, send_period),
   m_pin(pin),
   m_invertedScale(invertedScale),
   m_inputType(inputType)
{
  pinMode(m_pin, INPUT);
}

MqttSensor::~MqttSensor()
{}

void MqttSensor::doMeasure()
{
  int old_value = m_value;

  if(m_inputType == ANALOGTYPE)
  {
    int tempValue = analogRead(m_pin);
    if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
    else                m_value = map(tempValue, 0, 1024, 0, 100);
  }
  else if (m_inputType == DIGITALTYPE)
  {
    m_value = digitalRead(m_pin);
	  if(m_invertedScale) m_value = !m_value;
  }

  if(old_value != m_value)
  {
    m_valureChangedFlag = true;
  }
}

bool MqttSensor::doMeasureAndSendDataIfItsTime()
{
	if (isItTimeToSend())
	{
		doMeasure();

		return sendData();
	}
	else
	{
		return false;
	}
}

bool MqttSensor::doMeasureAndSendDataIfItsTimeAndValueChanged()
{
	if (isItTimeToSend())
	{
		doMeasure();

		return sendDataIfItsTimeAndValueChanged();
	}
	else
	{
		return false;
	}
}

bool MqttSensor::doMeasureIfItsTime()
{
	if (isItTimeToSend())
	{
		doMeasure();
		clearSendFlag();

		return true;
	}
	else
	{
		return false;
	}
}

EInputType MqttSensor::getInputType()
{
  return m_inputType;
}

EScaleType MqttSensor::getScaleType()
{
  return m_invertedScale;
}
