#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "dht11mqtt.h"
#include "Kuchnia-defines.h"
#include "Interface.h"

dht11mqtt::dht11mqtt(int tempTopic, int humidityTopic, int pin)
{
	m_sendFlag        = 0;
	m_mqttTempTopic   = tempTopic;
	m_mqttHumiTopic   = humidityTopic;
	m_pin             = pin;
}

void dht11mqtt::doMeasure()
{/*
  int chk = DHT11.read(DHT11PIN);

  switch (chk)
  {
    case DHTLIB_OK: 
                #ifdef DEBUG
		Serial.println("OK");
                #endif
		break;
    case DHTLIB_ERROR_CHECKSUM:
                #ifdef DEBUG 
		Serial.println("Checksum error"); 
                #endif
		break;
    case DHTLIB_ERROR_TIMEOUT: 
                #ifdef DEBUG
		Serial.println("Time out error"); 
                #endif
		break;
    default: 
                #ifdef DEBUG
		Serial.println("Unknown error"); 
                #endif
		break;
  }
  #ifdef DEBUG
  Serial.print("Humidity (%): ");
  Serial.println(DHTData[1]);
  Serial.print("Temperature (°C): ");
  Serial.println(DHTData[0]);
  #endif
*/  
  m_sendFlag = 1;
}

