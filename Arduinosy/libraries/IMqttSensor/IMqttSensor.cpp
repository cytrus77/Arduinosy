#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "IMqttSensor.h"
//#include "Utils.h"

#define DEBUG 1

IMqttSensor::IMqttSensor(int topic, PubSubClient* client, unsigned long send_period)
  : m_value(0),
    m_mqttTopic(topic),
    m_itsTimeToSendFlag(false),
    m_valureChangedFlag(false),
    m_sendPeriod(send_period),
    m_sendTimer(send_period),
    m_mqttClient(client)
{
}

IMqttSensor::~IMqttSensor()
{
}

int IMqttSensor::getMqttTopic()
{
  return m_mqttTopic;
}

int IMqttSensor::getValue()
{
  return m_value;
}

int* IMqttSensor::getValuePtr()
{
  return &m_value;
}

void IMqttSensor::processTimer()
{
  --m_sendTimer;

  if (m_sendTimer <= 0)
  {
    m_itsTimeToSendFlag = true;
    m_sendTimer = m_sendPeriod;
  }
}

bool IMqttSensor::isItTimeToSend()
{
  return m_itsTimeToSendFlag;
}

bool IMqttSensor::isValueChanged()
{
  return m_valureChangedFlag;
}

void IMqttSensor::clearSendFlag()
{
  m_sendTimer = m_sendPeriod;
  m_itsTimeToSendFlag = false;
}


bool IMqttSensor::sendDataIfValueChanged()
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

bool IMqttSensor::sendDataIfItsTime()
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

bool IMqttSensor::sendDataIfItsTimeAndValueChanged()
{
	if (m_valureChangedFlag && m_itsTimeToSendFlag)
	{
		return sendData();
	}
	else
	{
		clearSendFlag();

		return false;
	}
}

bool IMqttSensor::sendData()
{
	sendMqttPacket(m_mqttTopic, m_value);
	m_itsTimeToSendFlag = false;
	m_valureChangedFlag = false;
	return true;
}

void IMqttSensor::sendMqttPacket(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  m_mqttClient->publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT int");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}

void IMqttSensor::sendMqttPacket(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  m_mqttClient->publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT float");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}
