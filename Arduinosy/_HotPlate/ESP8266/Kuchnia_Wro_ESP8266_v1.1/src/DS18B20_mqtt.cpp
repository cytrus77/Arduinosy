#include "DS18B20_mqtt.h"

#define DEBUG 1

ds18b20mqtt::ds18b20mqtt(int topic, PubSubClient* client, DallasTemperature* ds18b20, unsigned long send_period)
  : IMqttSensor(topic, client, send_period),
    m_ds18b20(ds18b20),
    m_tempValue(0)
{
}

ds18b20mqtt::~ds18b20mqtt()
{
}

void ds18b20mqtt::doMeasure()
{
  float old_value = m_tempValue;

  m_ds18b20->requestTemperatures();
  m_tempValue = m_ds18b20->getTempCByIndex(0);

  if(old_value != m_value)
  {
    m_valureChangedFlag = true;
  }

  #ifdef DEBUG
  Serial.print("MqttTopic=");
  Serial.print(m_mqttTopic);
  Serial.print(" mqttSensor::doMeasure() Temperature=");
  Serial.println(m_tempValue);
  #endif
}

bool ds18b20mqtt::doMeasureAndSendDataIfItsTime()
{
	if (isItTimeToSend())
	{
		doMeasure();

		return ds18b20mqtt::sendData();
	}
	else
	{
		return false;
	}
}

bool ds18b20mqtt::doMeasureAndSendDataIfItsTimeAndValueChanged()
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

bool ds18b20mqtt::doMeasureIfItsTime()
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

bool ds18b20mqtt::sendData()
{
  #ifdef DEBUG
  Serial.println("ds18b20mqtt Sending MQTT");
  Serial.println(m_mqttTopic);
  Serial.println(m_tempValue);
  #endif
  
	sendMqttPacket(m_mqttTopic, m_tempValue);
	m_itsTimeToSendFlag = false;
	m_valureChangedFlag = false;

	return true;
}
