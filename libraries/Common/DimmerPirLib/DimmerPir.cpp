#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DimmerPir.h"


DimmerPir::DimmerPir(const String& mqttPirTopic, const String& mqttLightTopic,
                     Dimmer* dimmer, bool* pirStatus, MqttSensor* lightSensor)
  : m_mqttPirTopic(mqttPirTopic),
	m_mqttLightTriggerTopic(mqttLightTopic),
	m_pirOnFlag(true),
	m_currentPir(pirStatus),
	m_lightTrigger(50),
	m_dimmer(dimmer),
	m_lightSensor(lightSensor),
  m_pirSensor(0)
{
}

DimmerPir::DimmerPir(const String& mqttPirTopic, const String& mqttLightTopic,
                     Dimmer* dimmer, MqttSensor* pirSensor, MqttSensor* lightSensor)
  : m_mqttPirTopic(mqttPirTopic),
	m_mqttLightTriggerTopic(mqttLightTopic),
	m_pirOnFlag(true),
	m_currentPir(0),
	m_lightTrigger(50),
	m_dimmer(dimmer),
	m_lightSensor(lightSensor),
  m_pirSensor(pirSensor)
{
}

void DimmerPir::checkSensors()
{
	if (m_pirOnFlag)
	{
    bool moveDeteced = false;

    if (m_pirSensor)
    {
      m_pirSensor->doMeasure();
      moveDeteced = m_pirSensor->getValue();
    }
		else
		{
      moveDeteced = *m_currentPir;
		}

    if (moveDeteced)
    {
      if (m_dimmer->getValue() == 0)
      {
        m_lightSensor->doMeasure();

        if (m_lightSensor->getInputType() == ANALOGTYPE)
        {
          if (m_lightSensor->getValue() <= m_lightTrigger)
          {
            m_dimmer->setValue(100);
          }
        }
        else if (m_lightSensor->getInputType() == DIGITALTYPE)
        {
          if (m_lightSensor->getScaleType() == NORMALSCALE)
          {
            if (m_lightSensor->getValue())
            {
              m_dimmer->setValue(100);
            }
          }
          else if (m_lightSensor->getScaleType() == INVERTEDSCALE)
          {
            if (!m_lightSensor->getValue())
            {
              m_dimmer->setValue(100);
            }
          }
        }
      }
      else
      {
        m_dimmer->resetTimer();
      }
    }
	}
}

void DimmerPir::setLightTrigger(int light)
{
	m_lightTrigger = light;

	checkSensors();
}

void DimmerPir::setPirFlag(bool pirFlag)
{
	m_pirOnFlag = pirFlag;

	checkSensors();
}

const String& DimmerPir::getPirMqttTopic()
{
	return m_mqttPirTopic;
}

const String& DimmerPir::getLightMqttTopic()
{
	return m_mqttLightTriggerTopic;
}
