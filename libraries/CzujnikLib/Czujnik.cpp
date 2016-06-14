#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Czujnik.h"

sensor *sensor::sensorPtr[6];
int sensor::m_sensorCounter = 0;

sensor::sensor(int topic, int pin, bool invertedScale, bool inputType)
{
  m_sensorCounter++;
  m_timer         = 0;
  m_sendFlag      = 0;
  m_mqttTopic     = topic;
  m_pin           = pin;
  m_inputType     = inputType;
  m_invertedScale = invertedScale;
  sensorPtr[m_sensorCounter-1] = this;
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
  else if (m_inputType == DIGITALTYPE) m_value = digitalRead(m_pin);
  #ifdef DEBUG
  Serial.println("sensor::doMeasure()");
  #endif
  if(((abs(old_value - m_value) > 10 && m_inputType == ANALOGTYPE) || m_inputType == DIGITALTYPE) && old_value != m_value || getDiffTime(&m_timer, SENSORSENDPERIOD))
  {
    m_sendFlag = 1;
  }
}

