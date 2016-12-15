#pragma once

#include <Ethernet.h>

#define DEBUG 1
#define RELAY_PIN 5
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


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 0, 143 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port
String readString;
volatile int timerCounter = 0;
EShortCircuitState shortCircuitState;
bool addRefeshSection = true;
