#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <DallasTemperature.h>
#include <dht.h>
#include "DHTMqtt.h"
#include "Utils.h"

#define DEBUG 1

dhtMqtt::dhtMqtt(int tempTopic, int humiTopic, PubSubClient* client, int pin, unsigned long send_period)
  : IMqttSensor(humiTopic, client, send_period),
    m_tempValue(0),
    m_tempMqttTopic(tempTopic),
    m_pin(pin)
{
}

dhtMqtt::~dhtMqtt()
{
}

void dhtMqtt::doMeasure()
{
	float old_tempValue = m_tempValue;
	int old_value = m_value;

    for (static unsigned short faileCounter = 1; faileCounter < 2001; ++faileCounter)
    {
        int chk = read11(m_pin);

        if (chk == DHTLIB_OK)
        {
          #ifdef DEBUG
          Serial.print("DHT - OK - try: ");
          Serial.println(faileCounter);
          #endif

          m_tempValue = (float)temperature;
          m_value     = (int)humidity;
          break;
        }
        else
        {
          if (faileCounter%1000 == 0)
          {
            #ifdef DEBUG
        	Serial.print("DHT - WARNING NOK - attempt: ");
        	Serial.println(faileCounter);
            #endif
          }
        }
    }

    if(old_value != m_value && old_tempValue != m_tempValue)
    {
      m_valureChangedFlag = true;
    }
}

bool dhtMqtt::doMeasureIfItsTime()
{
	if (isItTimeToSend())
	{
		doMeasure();
		clearSendFlag();

		return true;
	}
	else
	{
		return false;
	}
}

bool dhtMqtt::doMeasureAndSendDataIfItsTime()
{
	if (isItTimeToSend())
	{
		doMeasure();

		return sendData();
	}
	else
	{
		return false;
	}
}
bool dhtMqtt::doMeasureAndSendDataIfItsTimeAndValueChanged()
{
	if (isItTimeToSend())
	{
		doMeasure();

		return sendDataIfItsTimeAndValueChanged();
	}
	else
	{
		return false;
	}
}

bool dhtMqtt::sendData()
{
	IMqttSensor::sendMqttPacket(m_mqttTopic, m_value);
	sendMqttPacket(m_tempMqttTopic, m_tempValue);
	m_itsTimeToSendFlag = false;
	m_valureChangedFlag = false;
	return true;
}

void dhtMqtt::sendMqttPacket(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  m_mqttClient->publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}

