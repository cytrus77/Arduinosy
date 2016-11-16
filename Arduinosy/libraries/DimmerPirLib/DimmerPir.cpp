#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "DimmerPir.h"

//#define DEBUG 1


dimmerPir::dimmerPir(int mqttPirTopic, int mqttLightTopic, dimmer* dimmer, bool* pirStatus, mqttSensor* lightSensor)
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

dimmerPir::dimmerPir(int mqttPirTopic, int mqttLightTopic, dimmer* dimmer, mqttSensor* pirSensor, mqttSensor* lightSensor)
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

void dimmerPir::checkSensors()
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
            Serial.println("Set light to 100 !");
          }
        }
        else if (m_lightSensor->getInputType() == DIGITALTYPE)
        {
          if (m_lightSensor->getScaleType() == NORMALSCALE)
          {
            if (m_lightSensor->getValue())
            {
              m_dimmer->setValue(100);
              Serial.println("Set light to 100 !");
            }
          }
          else if (m_lightSensor->getScaleType() == INVERTEDSCALE)
          {
            if (!m_lightSensor->getValue())
            {
              m_dimmer->setValue(100);
              Serial.println("Set light to 100 !");
            }
          }
        }
      }
      else
      {
        m_dimmer->resetTimer();
      }
    }

    #ifdef DEBUG
    Serial.print("DimmerPir values m_pirOnFlag=");
    Serial.print(m_pirOnFlag);
    Serial.print(" dimmerValue=");
    Serial.print(m_dimmer->getValue());
    Serial.print(" moveDeteced=");
    Serial.print(moveDeteced);
    Serial.print(" m_currentLight=");
    Serial.print(m_lightSensor->getValue());
    Serial.print(" m_lightTrigger=");
    Serial.println(m_lightTrigger);
    #endif
	}
}

void dimmerPir::setLightTrigger(int light)
{
	m_lightTrigger = light;

	checkSensors();
}

void dimmerPir::setPirFlag(bool pirFlag)
{
	m_pirOnFlag = pirFlag;

	checkSensors();
}

int dimmerPir::getPirMqttTopic()
{
	return m_mqttPirTopic;
}

int dimmerPir::getLightMqttTopic()
{
	return m_mqttLightTriggerTopic;
}
