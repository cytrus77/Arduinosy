#ifndef DimmerPIR_H
#define DimmerPIR_H

#include "Dimmer.h"
#include "MqttSensor.h"


class DimmerPir
{
private:
	String m_mqttPirTopic;
	String m_mqttLightTriggerTopic;
	bool  m_pirOnFlag;
	bool* m_currentPir;
	int   m_lightTrigger;
	Dimmer* m_dimmer;
	MqttSensor* m_lightSensor;
	MqttSensor* m_pirSensor;

public:
	DimmerPir(const String& mqttPirTopic, const String& mqttLightTopic,
		        Dimmer* dimmer, bool* pirStatus, MqttSensor* lightSensor);
	DimmerPir(const String& mqttPirTopic, const String& mqttLightTopic,
		        Dimmer* dimmer, MqttSensor* pirSensor, MqttSensor* lightSensor);

	void checkSensors();

	const String& getPirMqttTopic();
	const String& getLightMqttTopic();

	void setLightTrigger(int light);
	void setPirFlag(bool pirFlag);
};

#endif
