#pragma once

#include <Ethernet.h>

#define DEBUG 1
#define RELAY_PIN 5
#define MSG_BUFFER_SIZE 1
#define SERIAL_TIMEOUT 20   // 20ms
#define TIMER0_PERIOD  50000 // in us
#define TIMER_COUNT_1S 1000000/TIMER0_PERIOD
#define TIMER_COUNT_2S 2*TIMER_COUNT_1S
#define TIMER_COUNT_5S 5*TIMER_COUNT_1S
#define MAX_STRING_LENGTH 5

struct MsgBuffer
{
  char readString[MSG_BUFFER_SIZE][MAX_STRING_LENGTH];
  char counter = {0};
};

enum ERs485State
{
  Rs485Defined = 1,
  Rs485Loopback,
  Rs485RxTest,
  Rs485TxTest,
  Rs485NoResponse
};

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
MsgBuffer msgBuffer;
ERs485State rs485State;
EShortCircuitState shortCircuitState;
bool addRefeshSection = true;
bool sendString = false;
