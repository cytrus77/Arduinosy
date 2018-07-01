#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "IMqttSensor.h"

IMqttSensor::IMqttSensor(const String& topic, PubSubClient* client, unsigned long send_period)
  : m_value(0),
    m_mqttTopic(topic),
    m_itsTimeToSendFlag(false),
    m_valureChangedFlag(false),
    m_sendPeriod(send_period),
    m_sendTimer(send_period),
    m_mqttClient(client)
{}

IMqttSensor::~IMqttSensor()
{}

const String& IMqttSensor::getMqttTopic()
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
	if (m_valureChangedFlag) return sendData();
	else return false;
}

bool IMqttSensor::sendDataIfItsTime()
{
	if (m_valureChangedFlag) return sendData();
	else return false;
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
  Serial.print("sendMqttPacket Topic=");
  Serial.println(m_mqttTopic.c_str());
	sendMqttPacket(m_mqttTopic, m_value);
	m_itsTimeToSendFlag = false;
	m_valureChangedFlag = false;
	return true;
}

void IMqttSensor::sendMqttPacket(const String& topic, int value)
{
  char dataChar[10];
  itoa(value, dataChar, 10);
  m_mqttClient->publish(topic.c_str(), dataChar);
  Serial.print("Topic=");
  Serial.print(topic.c_str());
  Serial.print(" value=");
  Serial.println(value);
}

void IMqttSensor::sendMqttPacket(const String& topic, float value)
{
  char dataChar[10];
  ftoa(value, dataChar);
  m_mqttClient->publish(topic.c_str(), dataChar);
  Serial.print("Topic=");
  Serial.print(topic.c_str());
  Serial.print(" value=");
  Serial.println(value);
}
