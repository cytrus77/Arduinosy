#ifndef DIMMERPIR_H
#define DIMMERPIR_H

#include "Dimmer.h"
#include "Czujnik.h"

class dimmerPir
{
private:
  int m_trigger;
  int m_setValue;
  int m_timeout;
  unsigned long m_startTimer;
  int m_currentValue;
  int m_currentLight;
  int m_pin;
  bool m_pir_flag;

public:
  dimmer(int pin);
  void setDimmer(void);
  void setTrigger(int trigger);
  void setValue(int value);
  void setTimeout(int timeout);
  void setCurrentLight(int light);
  void checkPir();
  void checkTimeout();
  void resetTimer();
  void setPirFlag(bool flag);
};

#endif