#ifndef DEFINES_H
#define DEFINES_H

// PINOUT
// PIN 0 - TX serial
// PIN 1 - RX serial
#define ZERODETECTPIN        2
//#define NOTUSED            3
//#define NOTUSED            4
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
#define TIMER0PERIOD         500 // 500us
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
