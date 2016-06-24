#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DallasTemperatureMqtt.h>
#include <dht.h>
#include "DHTMqtt.h"
#include "Utils.h"

#define DEBUG 1

DallasTemperatureMqtt::DallasTemperatureMqtt(int topic, PubSubClient* client, OneWire* oneWire, unsigned long send_period)
  : IMqttSensor(topic, client, send_period),
    DallasTemperature(oneWire),
    m_value(0)
{
}

DallasTemperatureMqtt::~DallasTemperatureMqtt()
{
}

void DallasTemperatureMqtt::doMeasure()
{
	float old_value = m_value;
    requestTemperatures();
    m_value = getTempCByIndex(0);

    if(old_value != m_value)
    {
      m_valureChangedFlag = true;
    }
}

bool DallasTemperatureMqtt::doMeasureIfItsTime()
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

bool DallasTemperatureMqtt::doMeasureAndSendDataIfItsTime()
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
bool DallasTemperatureMqtt::doMeasureAndSendDataIfItsTimeAndValueChanged()
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

bool DallasTemperatureMqtt::sendData()
{
	sendMqttPacket(m_mqttTopic, m_value);
	m_itsTimeToSendFlag = false;
	m_valureChangedFlag = false;
	return true;
}

void DallasTemperatureMqtt::sendMqttPacket(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  m_mqttClient->publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}
