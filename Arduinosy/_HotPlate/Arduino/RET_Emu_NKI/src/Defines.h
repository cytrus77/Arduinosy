#pragma once

#include <Ethernet.h>

#define DEBUG 1

#define NUM_OF_PORTS  5

#define RELAY1_PIN    2
#define RELAY2_PIN    3
#define RELAY3_PIN    4
#define RELAY4_PIN    5
#define RELAY5_PIN    6

#define DCVOL1_PIN  A0
#define DCVOL2_PIN  A1
#define DCVOL3_PIN  A2
#define DCVOL4_PIN  A3
#define DCVOL5_PIN  A4

#define TIMER0_PERIOD  50000 // in us
#define TIMER_COUNT_1S 1000000/TIMER0_PERIOD
#define TIMER_COUNT_2S 2*TIMER_COUNT_1S
#define TIMER_COUNT_5S 5*TIMER_COUNT_1S

enum EShortCircuitState
{
  ShortCircuitOff = 1,
  ShortCircuit2s,
  ShortCircuit5s,
  ShortCircuitPerm
};

enum ERelayState
{
  Off = 0,
  On
};

struct SPort
{
  const int relayPin;
  const int dcVolPin;
  volatile EShortCircuitState shortState;
  volatile ERelayState relayState;
  volatile unsigned int timer;

  SPort(const int relay, const int dcVol)
    : relayPin(relay)
    , dcVolPin(dcVol)
    , shortState(ShortCircuitOff)
    , relayState(Off)
    , timer(0)
  {}
};

SPort port[NUM_OF_PORTS] = {
  {RELAY1_PIN, DCVOL1_PIN},
  {RELAY2_PIN, DCVOL2_PIN},
  {RELAY3_PIN, DCVOL3_PIN},
  {RELAY4_PIN, DCVOL4_PIN},
  {RELAY5_PIN, DCVOL5_PIN}
};

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 0, 143 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port

String readString;
bool addRefeshSection = true;
