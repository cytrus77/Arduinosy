#ifndef DEFINES_H
#define DEFINES_H

// PINOUT
// PIN 0 - TX serial
// PIN 1 - RX serial
#define ZERODETECTPIN        2
#define MQTTRXPIN            3
#define MQTTTXPIN            4
#define ROLLERUPPIN          5
#define ROLLERDOWNPIN        6
//#define NOTUSED            7
#define SWITCHDOWNPIN        8
#define SWITCHUPPIN          9
//#define NOTUSED            10
//#define NOTUSED            11
#define MQTTROLLERDOWNPIN    12
#define MQTTROLLERUPPIN      13
//#define NOTUSED            A0
//#define NOTUSED            A1
//#define NOTUSED            A2
//#define NOTUSED            A3
//#define NOTUSED            A4
//#define NOTUSED            A5

// measurement period
const unsigned int  TIMER0PERIOD = 1000;  // 1ms
const unsigned long COUNTIN1SEC  = 1000000/TIMER0PERIOD;
const unsigned long COUNTIN1MS   = 1000/TIMER0PERIOD;
const unsigned int MQTTRXTIMEOUT = 50;   // 50ms
const unsigned int MQTTTXTIMEOUT = 3;    // 3s
const unsigned int ROLLERTIMEOUT = 30;   // 30s
const unsigned int BUTTONTIMEOUT = 5;    // 5s

enum ERollerState
{
  ERollerNone,
  ERollerStop,
  ERollerUp,
  ERollerDown
};

enum EInputStatus
{
  ENone,
  ESwitchUp,
  ESwitchDown,
  ESwitchBoth,
  ESwitchUpLong,
  ESwitchDownLong,
  ESwitchBothLong,
  EMqttUp,
  EMqttDown,
  EMqttStop
};

#endif
