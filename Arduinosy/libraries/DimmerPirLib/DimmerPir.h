#ifndef DIMMERPIR_H
#define DIMMERPIR_H

#include "Dimmer.h"
#include "MqttSensor.h"


class dimmerPir
{
private:
	int   m_mqttPirTopic;
	int   m_mqttLightTriggerTopic;
	bool  m_pirOnFlag;
	bool* m_currentPir;
	int   m_lightTrigger;
	dimmer* m_dimmer;
	mqttSensor* m_lightSensor;

public:
	dimmerPir(int mqttPirTopic, int mqttLightTopic, dimmer* dimmer, bool* pirStatus, mqttSensor* lightSensor);

	void checkSensors();

	int getPirMqttTopic();
	int getLightMqttTopic();

	void setLightTrigger(int light);
	void setPirFlag(bool pirFlag);
};

#endif