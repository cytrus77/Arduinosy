#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Czujnik.h"


sensor::sensor(int topic, int pin, bool invertedScale, bool inputType, unsigned long send_period)
{
  m_value         = 0;
  m_mqttTopic     = topic;
  m_pin           = pin;
  m_inputType     = inputType;
  m_invertedScale = invertedScale;
  m_sendPeriod    = send_period;
  m_sendTimer     = m_sendPeriod;
  m_sendFlag      = false;
  pinMode(m_pin, INPUT);
}

void sensor::doMeasure()
{
  int old_value = m_value;

  if(m_inputType == ANALOGTYPE)
  {
    int tempValue = analogRead(m_pin);
    if(m_invertedScale) m_value = map(tempValue, 0, 1024, 100, 0);
    else                m_value = map(tempValue, 0, 1024, 0, 100);
  }
  else if (m_inputType == DIGITALTYPE) 
  {
    m_value = digitalRead(m_pin);
	if(m_invertedScale) m_value = !m_value;
  }
  #ifdef DEBUG
  Serial.print("MqttTopic=");
  Serial.print(m_mqttTopic);
  Serial.print(" sensor::doMeasure() value=");
  Serial.println(m_value);
  #endif
}

int sensor::getMqttTopic()
{
  return m_mqttTopic;
}

int sensor::getValue()
{
  return m_value;
}

void sensor::checkTimer()
{
  --m_sendTimer;

  if (m_sendTimer <= 0)
  {
    m_sendFlag = true;
  }
}

bool sensor::isDataSend()
{
  return m_sendFlag;
}

