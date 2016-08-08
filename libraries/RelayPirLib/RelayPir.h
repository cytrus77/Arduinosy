#ifndef RELAYPIR_H
#define RELAYPIR_H

#include "Relay.h"
#include "MqttSensor.h"


class relayPir
{
private:
	int   m_mqttPirTopic;
	int   m_mqttLightTriggerTopic;
	bool  m_pirOnFlag;
	bool* m_currentPir;
	int   m_lightTrigger;
	relay* m_relay;
	mqttSensor* m_lightSensor;

public:
	relayPir(int mqttPirTopic, int mqttLightTopic, relay* relay, bool* pirStatus, mqttSensor* lightSensor);

	void checkSensors();

	int getPirMqttTopic();
	int getLightMqttTopic();

	void setLightTrigger(int light);
	void setPirFlag(bool pirFlag);
};

#endif
