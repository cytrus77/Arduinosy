#pragma once

#include <Ethernet.h>

#define RS485_RX_DIRECTION_PIN 2   //RS485 Direction control
#define RS485_TX_DIRECTION_PIN 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define MSG_BUFFER_SIZE   10
#define SERIAL_TIMEOUT    20   // 20ms
#define MAX_STRING_LENGTH 25
#define TIMER0_PERIOD     50000 // in us
#define TIMER_COUNT_1S    1000000/TIMER0_PERIOD
#define TIMER_COUNT_2S    2*TIMER_COUNT_1S
#define TIMER_COUNT_5S    5*TIMER_COUNT_1S

struct MsgBuffer
{
  char readString[MSG_BUFFER_SIZE][MAX_STRING_LENGTH];
  char counter = {0};
};

enum ERs485State
{
  Rs485Loopback = 1,
  Rs485RxTest,
  Rs485TxTest
};


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xCA };   //physical mac address
byte ip[] = { 192, 168, 0, 144 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port
String readString;
volatile int timerCounter = 0;
MsgBuffer msgBuffer;
ERs485State rs485State;
bool addRefeshSection = true;
bool sendString = false;
