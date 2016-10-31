#ifndef DEFINES_H
#define DEFINES_H

// PINOUT
// PIN 0 - TX serial
// PIN 1 - RX serial
#define ROLLERUPPIN          5
#define ROLLERDOWNPIN        6
//#define SSforSDCard        4 - reserved
#define SWITCHUPPIN          9
#define SWITCHDOWNPIN        8
#define MQTTROLLERUPPIN      13
#define MQTTROLLERDOWNPIN    12
//#define NOTUSED             9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
//#define NOTUSED            A0
//#define NOTUSED            A1
//#define NOTUSED            A2
//#define NOTUSED            A3
//#define NOTUSED            A4
//#define NOTUSED            A5

// measurement period
#define TIMER0PERIOD         2000 // 2ms
#define COUNTIN1SEC          1000000/TIMER0PERIOD
#define ROLLERTIMEOUT        30   // 30s

enum ERollerState
{
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
  EMqttUp,
  EMqttDown
};

#endif
