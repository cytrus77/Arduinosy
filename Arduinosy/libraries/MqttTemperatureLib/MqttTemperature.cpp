#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DallasTemperature.h>
#include <dht.h>
#include "MqttTemperature.h"

#define DEBUG 1

MqttTemperature::MqttTemperature(int tempTopic, PubSubClient* client, DallasTemperature* ds18b20, unsigned long send_period)
{
}

MqttTemperature::MqttTemperature(int tempTopic, int humiTopic, PubSubClient* client, dht* dht, unsigned long send_period)
  : m_value(0),
   // m_mqttTopic(topic),
    m_itsTimeToSnedFlag(false),
    m_valureChangedFlag(false),
    m_sendPeriod(send_period),
    m_sendTimer(send_period),
    m_mqttClient(client)
{
}

MqttTemperature::~MqttTemperature()
{
}

int MqttTemperature::getMqttTopic()
{
  return m_mqttTopic;
}

int MqttTemperature::getValue()
{
  return m_value;
}

int* MqttTemperature::getValuePtr()
{
  return &m_value;
}

void MqttTemperature::processTimer()
{
  --m_sendTimer;

  if (m_sendTimer <= 0)
  {
	m_itsTimeToSnedFlag = true;
    m_sendTimer = m_sendPeriod;
  }
}

bool MqttTemperature::isItTimeToSend()
{
  return m_itsTimeToSnedFlag;
}

bool MqttTemperature::isValueChanged()
{
  return m_valureChangedFlag;
}

void MqttTemperature::clearSendFlag()
{
  m_sendTimer = m_sendPeriod;
  m_itsTimeToSnedFlag = false;
}


bool MqttTemperature::sendDataIfValueChanged()
{
	if (m_valureChangedFlag)
	{
		return sendData();
	}
	else
	{
		return false;
	}
}

bool MqttTemperature::sendDataIfItsTime()
{
	if (m_valureChangedFlag)
	{
		return sendData();
	}
	else
	{
		return false;
	}
}

bool MqttTemperature::sendDataIfItsTimeAndValueChanged()
{
	if (m_valureChangedFlag && m_itsTimeToSnedFlag)
	{
		return sendData();
	}
	else
	{
		return false;
	}
}

bool MqttTemperature::sendData()
{
	sendMqttPacket(m_mqttTopic, m_value);
	m_itsTimeToSnedFlag = false;
	m_valureChangedFlag = false;
	return true;
}

void MqttTemperature::sendMqttPacket(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  m_mqttClient->publish(topicChar, dataChar);
  
  #ifdef DEBUG
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}
