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
	int   m_currentLight;
	dimmer* m_dimmer;

public:
	dimmerPir(int mqttPirTopic, int mqttLightTopic, dimmer* dimmer, bool* pirStatus);

	void checkSensors();

	int getPirMqttTopic();
	int getLightMqttTopic();

	void setLightTrigger(int light);
	void setCurrentLight(int light);
	void setPirFlag(bool pirFlag);
};

#endif
