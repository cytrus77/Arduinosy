//#include <dht11.h>

#ifndef DHT11MQTT_H
#define DHT11MQTT_H

class dht11mqtt //: public dht11
{
private:
	int  m_tempValue;
	int  m_humiValue;
	int  m_mqttTempTopic;
	int  m_mqttHumiTopic;
	int  m_pin;
	int  m_sendFlag;

public:
	dht11mqtt(int tempTopic, int humidityTopic, int pin);
	void doMeasure();
};

#endif

