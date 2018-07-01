#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "RelayPir.h"

//#define DEBUG 1


relayPir::relayPir(int mqttPirTopic, int mqttLightTopic, relay* relay, bool* pirStatus, mqttSensor* lightSensor)
  : m_mqttPirTopic(mqttPirTopic),
	m_mqttLightTriggerTopic(mqttLightTopic),
	m_pirOnFlag(true),
	m_currentPir(pirStatus),
	m_lightTrigger(50),
	m_relay(relay),
	m_lightSensor(lightSensor)
{
}

void relayPir::checkSensors()
{
	if (m_pirOnFlag)
	{
		if (*m_currentPir && (m_lightSensor->getValue() <= m_lightTrigger))
		{
			m_relay->setValue(HIGH);
		}
	}

	#ifdef DEBUG
	Serial.print("relayPir values m_pirOnFlag=");
	Serial.print(m_pirOnFlag);
	Serial.print(" m_currentPir=");
	Serial.print(*m_currentPir);
	Serial.print(" m_currentLight=");
	Serial.print(m_currentLight);
	Serial.print(" m_lightTrigger=");
	Serial.println(m_lightTrigger);
	#endif
}

void relayPir::setLightTrigger(int light)
{
	m_lightTrigger = light;

	checkSensors();
}

void relayPir::setPirFlag(bool pirFlag)
{
	m_pirOnFlag = pirFlag;

	checkSensors();
}

int relayPir::getPirMqttTopic()
{
	return m_mqttPirTopic;
}

int relayPir::getLightMqttTopic()
{
	return m_mqttLightTriggerTopic;
}
