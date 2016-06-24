#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DimmerPir.h"

//#define DEBUG 1


dimmerPir::dimmerPir(int mqttPirTopic, int mqttLightTopic, dimmer* dimmer, bool* pirStatus)
  : m_mqttPirTopic(mqttPirTopic),
	m_mqttLightTriggerTopic(mqttLightTopic),
	m_pirOnFlag(true),
	m_currentPir(pirStatus),
	m_lightTrigger(50),
	m_currentLight(0),
	m_dimmer(dimmer)
{
}

void dimmerPir::checkSensors()
{
	if (m_pirOnFlag)
	{
		if (*m_currentPir && (m_currentLight <= m_lightTrigger))
		{
			if (m_dimmer->getValue() == 0)
			{
				m_dimmer->setValue(100);
			}
		}
	}

	#ifdef DEBUG
	Serial.print("DimmerPir values m_pirOnFlag=");
	Serial.print(m_pirOnFlag);
	Serial.print(" m_currentPir=");
	Serial.print(*m_currentPir);
	Serial.print(" m_currentLight=");
	Serial.print(m_currentLight);
	Serial.print(" m_lightTrigger=");
	Serial.println(m_lightTrigger);
	#endif
}

void dimmerPir::setLightTrigger(int light)
{
	m_lightTrigger = light;
}

void dimmerPir::setCurrentLight(int light)
{
	m_currentLight = light;
}

void dimmerPir::setPirFlag(bool pirFlag)
{
	m_pirOnFlag = pirFlag;
}

int dimmerPir::getPirMqttTopic()
{
	return m_mqttPirTopic;
}

int dimmerPir::getLightMqttTopic()
{
	return m_mqttLightTriggerTopic;
}
