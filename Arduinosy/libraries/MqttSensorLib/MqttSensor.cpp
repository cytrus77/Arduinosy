#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "MqttSensor.h"

//#define DEBUG 1

mqttSensor::mqttSensor(int topic, PubSubClient* client, int pin, EInputType inputType, EScaleType invertedScale, unsigned long send_period)
  : IMqttSensor(topic, client, send_period),
    m_pin(pin),
    m_invertedScale(invertedScale),
    m_inputType(inputType)
{
  pinMode(m_pin, INPUT);
}

mqttSensor::~mqttSensor()
{
}

void mqttSensor::doMeasure()
{
  int old_value = m_value;

  if(m_inputType == ANALOGTYPE)
  {
	analogRead(m_pin);
    int tempValue = analogRead(m_pin);
	#ifdef DEBUG
	Serial.print("ANALOG - tempValue=");
	Serial.println(tempValue);
	#endif
    if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
    else                m_value = map(tempValue, 0, 1024, 0, 100);
  }
  else if (m_inputType == DIGITALTYPE) 
  {
    digitalRead(m_pin);
    m_value = digitalRead(m_pin);
	#ifdef DEBUG
	Serial.print("DIGITAL - tempValue=");
	Serial.println(m_value);
	#endif
	if(m_invertedScale) m_value = !m_value;
  }

  if(old_value != m_value)
  {
    m_valureChangedFlag = true;
  }

  #ifdef DEBUG
  Serial.print("MqttTopic=");
  Serial.print(m_mqttTopic);
  Serial.print(" mqttSensor::doMeasure() value=");
  Serial.println(m_value);
  #endif
}

bool mqttSensor::doMeasureAndSendDataIfItsTime()
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

bool mqttSensor::doMeasureAndSendDataIfItsTimeAndValueChanged()
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

bool mqttSensor::doMeasureIfItsTime()
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

