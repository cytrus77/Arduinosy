#include "TimerOne.h"
#include <avr/wdt.h>

const int LED_PIN   = 3;
const int RELAY_PIN = 5;

const int serialReadTimeout = 300;
const long reqSendPeriod = 1000000;

const char headerChar = 0x2A;     // *
const char delimiterChar = 0x20;  // space
const char terminatorChar = 0x0D; // CR

// Request in HEX: 2A 20 47 45 54 5F 56 41 4C 55 45 20 53 54 41 54 55 53 20 0D 
const String getValue = "GET_VALUE";
const String statusCmd = "STATUS";
const String recordResp = "RECORDING";
const String streamResp = "STREAMING";

bool sendStatusReqFlag = false;

void setup()
{
  setLed(true);
  setRelay(false);
  Serial.begin(9600);
  Serial.setTimeout(serialReadTimeout);
  
  pinMode(LED_PIN,   OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Timer1.initialize(reqSendPeriod);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_2S);

  setLed(false);
}

void loop()
{
  wdt_reset();
  
  if (sendStatusReqFlag)
  {
    setLed(true);
    sendGetStatusRequest();
    sendStatusReqFlag = false;
  }

  setLed(false);

  while (Serial.available() > 0)
  {
    setLed(true);
    String resp = Serial.readString();

    if(resp.indexOf(getValue + delimiterChar + statusCmd) >= 0)
    {
      if(resp.indexOf(statusCmd + String('=') + recordResp) > 0 ||
         resp.indexOf(statusCmd + String('=') + streamResp) > 0)
      {
        setRelay(true);
      }
      else
      {
        setRelay(false);
      }
    }
  }
}

void TimerLoop()
{
  sendStatusReqFlag = true;
}

void sendGetStatusRequest(void)
{
  String msg = String(headerChar) + delimiterChar + getValue + delimiterChar 
               + statusCmd + delimiterChar + terminatorChar;
  Serial.print(msg);
}

void setRelay(bool value) // true = on ; false = off
{
  if (value) digitalWrite(RELAY_PIN, LOW);
  else digitalWrite(RELAY_PIN, HIGH);
}

void setLed(bool value) // true = on ; false = off
{
  if (value) digitalWrite(LED_PIN, HIGH);
  else digitalWrite(LED_PIN, LOW);
}

