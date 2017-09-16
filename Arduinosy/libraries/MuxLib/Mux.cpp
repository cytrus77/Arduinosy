#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Mux.h"

Mux::Mux(int en, int s0, int s1, int s2, int s3, int signal, SignalMode mode)
  : m_en_pin(en)
  , m_s0_pin(s0)
  , m_s1_pin(s1)
  , m_s2_pin(s2)
  , m_s3_pin(s3)
  , m_signal_pin(signal)
  , mode(mode)
{
  pinMode(m_en_pin, OUTPUT);
  pinMode(m_s0_pin, OUTPUT);
  pinMode(m_s1_pin, OUTPUT);
  pinMode(m_s2_pin, OUTPUT);
  pinMode(m_s3_pin, OUTPUT);
  pinMode(m_signal_pin, mode == Input ? INPUT : OUTPUT);

  digitalWrite(m_en_pin, HIGH);
}

void Mux::setOutput(int channel, bool state)
{
  if (mode == Output)
  {
    digitalWrite(m_s0_pin, channel & B0001);
    digitalWrite(m_s1_pin, channel & B0010);
    digitalWrite(m_s2_pin, channel & B0100);
    digitalWrite(m_s3_pin, channel & B1000);
    digitalWrite(m_en_pin, LOW);
    digitalWrite(m_signal_pin, state);
    delay(1000);
    digitalWrite(m_en_pin, HIGH);
  }
}

bool Mux::getInput(int channel)
{
  bool state = false;

  if (mode == Input)
  {
    digitalWrite(m_s0_pin, channel & B0001);
    digitalWrite(m_s1_pin, channel & B0010);
    digitalWrite(m_s2_pin, channel & B0100);
    digitalWrite(m_s3_pin, channel & B1000);
    digitalWrite(m_en_pin, LOW);
    state = digitalRead(m_signal_pin);
    digitalWrite(m_en_pin, HIGH);
  }

  return state;
}
