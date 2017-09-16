#ifndef MUX_H
#define MUX_H

class Mux
{
public:
  enum SignalMode : int
  {
    Output = 0,
    Input  = 1
  };

  Mux(int en, int s0, int s1, int s2, int s3, int signal, SignalMode mode);
  void setOutput(int channel, bool state);
  bool getInput(int channel);

private:
  int  m_en_pin;
  int  m_s0_pin;
  int  m_s1_pin;
  int  m_s2_pin;
  int  m_s3_pin;
  int  m_signal_pin;
  SignalMode mode;
};

#endif
