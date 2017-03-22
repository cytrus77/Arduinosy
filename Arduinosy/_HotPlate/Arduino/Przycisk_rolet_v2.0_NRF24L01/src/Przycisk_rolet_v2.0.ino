#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Defines.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG 1

RF24 radio(NRF24L01_CE, NRF24L01_CSN);
const byte rxAddrBrdc[6] = "00000";
const byte rxAddr1[6] = "00001";
const byte rxAddr2[6] = "00002";

const unsigned char MQTT_UP_REQ   = B0011;
const unsigned char MQTT_DOWN_REQ = B0101;
const unsigned char MQTT_STOP_REQ = B0110;

ERollerState targetRollerState  = ERollerStop;
ERollerState currentRollerState = ERollerNone;
EInputStatus globalInput = ENone;

unsigned long RollerTimer = 0;
unsigned long MqttRxTimer = 0;
unsigned long MqttTxTimer = 0;
unsigned long ButtonTimer = 0;
const unsigned long RollerTimeout = ROLLERTIMEOUT * COUNTIN1SEC;
const unsigned long MqttRxTimeout = MQTTRXTIMEOUT * COUNTIN1MS;
const unsigned long MqttTxTimeout = MQTTTXTIMEOUT * COUNTIN1SEC;
const unsigned long ButtonTimeout = BUTTONTIMEOUT * COUNTIN1SEC;
bool zeroDetected = false;
unsigned char mqttRecived = 0;


///////////////////////////////START SETUP//////////////////////////////////////
void setup() {
  Serial.begin(19200);

  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);

  pinMode(ROLLERUPPIN, OUTPUT);
  pinMode(ROLLERDOWNPIN, OUTPUT);
  pinMode(SWITCHUPPIN, INPUT_PULLUP);
  pinMode(SWITCHDOWNPIN, INPUT_PULLUP);
  pinMode(ZERODETECTPIN, INPUT_PULLUP);

  RollerStop();

  Nrf24l01Config();

  wdt_enable(WDTO_1S);

  #ifdef DEBUG
  Serial.println("ARDUINO: system started");
  #endif

  delay(10);
}
///////////////////////////////////END SETUP////////////////////////////////////

/////////////////////////////////START MAIN LOOP////////////////////////////////
void loop() {
  wdt_reset();

  if (ENone == globalInput)
  {
    globalInput = CheckButton();
  }
  else
  {
    setRollerOutput(globalInput);
    globalInput = ENone;
  }

  if (currentRollerState != targetRollerState) turnOnZeroDetection();

  #ifdef DEBUG
  static int licznik = 0;
  ++licznik;

  if (licznik > 20)
  {
    Serial.print("globalInput = ");
    Serial.println(globalInput);
    Serial.print("targetRollerState = ");
    Serial.println(targetRollerState);
    Serial.print("currentRollerState = ");
    Serial.println(currentRollerState);
    Serial.print("RollerTimer = ");
    Serial.println(RollerTimer);
    licznik = 0;
  }
  #endif

  ReceiveMqttMsg();
}
//////////////////////////////END MAIN LOOP/////////////////////////////////////

void TimerLoop()
{
  if (ProcessRollerTimer()) targetRollerState = ERollerStop;
  ProcessRelayDelay();
  ProcessButtonTimer();
}

/////////////////////////////// ROLLER PART ////////////////////////////////////
bool setRollerOutput(EInputStatus input)
{
  switch(input)
  {
    case ESwitchUpLong:
    case ESwitchDownLong:
    case ESwitchBothLong:
    {
      unsigned long msg = 0xF1A2;
      SendMqttMsg(rxAddrBrdc, msg);
      // sendMqttReq(input);
    }
    break;

    case ESwitchUp:
    case EMqttUp:
      if (ERollerStop == targetRollerState)
      {
        StartRollerTimer();
        targetRollerState = ERollerUp;
      }
      else
      {
        targetRollerState = ERollerStop;
        StopRollerTimer();
      }
    break;

    case ESwitchDown:
    case EMqttDown:
      if (ERollerStop == targetRollerState)
      {
        StartRollerTimer();
        targetRollerState = ERollerDown;
      }
      else
      {
        targetRollerState = ERollerStop;
        StopRollerTimer();
      }
    break;

    case ESwitchBoth:
    case EMqttStop:
    case ENone:
    default:
      targetRollerState = ERollerStop;
      StopRollerTimer();
    break;
  }

  return true;
}

void RollerUp()
{
  // ROLLERUPPIN          5
  // ROLLERDOWNPIN        6
  // digitalWrite(ROLLERUPPIN, LOW);
  // digitalWrite(ROLLERDOWNPIN, HIGH);
  PORTD |= B01000000;
  PORTD &= B11011111;
  currentRollerState = ERollerUp;
  #ifdef DEBUG
  Serial.println("RollerUp");
  #endif
}

void RollerDown()
{
  // ROLLERUPPIN          5
  // ROLLERDOWNPIN        6
  // digitalWrite(ROLLERUPPIN, HIGH);
  // digitalWrite(ROLLERDOWNPIN, LOW);
  PORTD |= B00100000;
  PORTD &= B10111111;
  currentRollerState = ERollerDown;
  #ifdef DEBUG
  Serial.println("RollerDown");
  #endif
}

void RollerStop()
{
  // ROLLERUPPIN          5
  // ROLLERDOWNPIN        6
  // digitalWrite(ROLLERUPPIN, HIGH);
  // digitalWrite(ROLLERDOWNPIN, HIGH);
  PORTD |= B01100000;
  currentRollerState = ERollerStop;
  #ifdef DEBUG
  Serial.println("RollerStop");
  #endif
}

void turnOnZeroDetection()
{ attachInterrupt(digitalPinToInterrupt(ZERODETECTPIN), zeroDetection, RISING); }

void turnOffZeroDetection()
{ detachInterrupt(digitalPinToInterrupt(ZERODETECTPIN)); }

void zeroDetection()
{
  static char zeroCounter = 0;
  ++zeroCounter;

  if (zeroCounter == 5)
  {
    zeroDetected = digitalRead(ZERODETECTPIN);
    zeroCounter = 0;
    turnOffZeroDetection();
  }
}

void ProcessRelayDelay()
{
  if (/*zeroDetected && */currentRollerState != targetRollerState)
  {
    static int relayCount = 0;

    if (relayCount < 5) ++relayCount;
    else
    {
      if (ERollerUp == targetRollerState)
      {
        RollerUp();
        StartRollerTimer();
      }
      else if (ERollerDown == targetRollerState)
      {
        RollerDown();
        StartRollerTimer();
      }
      else
      {
        RollerStop();
        StopRollerTimer();
      }

      relayCount = 0;
      zeroDetected = false;
    }
  }
}

void StartRollerTimer()
{ RollerTimer = RollerTimeout; }

void StopRollerTimer()
{ RollerTimer = 0; }

bool ProcessRollerTimer()
{
  if (RollerTimer)
  {
    --RollerTimer;
    if (RollerTimer == 0) return true;
  }
  return false;
}
//////////////////////////////// ROLLER PART END ///////////////////////////////

////////////////////////////////// MQTT PART ///////////////////////////////////
void Nrf24l01Config()
{
  #ifdef DEBUG
  Serial.println( "Starting wireless..." );
  #endif

  // Setup
  radio.begin();
  radio.setRetries(15, 15);

  radio.openReadingPipe(0, rxAddrBrdc);
  radio.openReadingPipe(1, rxAddr1);
  radio.stopListening();

  #ifdef DEBUG
  Serial.println( "Wireless initialized!" );
  #endif
}

void SendMqttMsg(byte* target, unsigned long& payload)
{
  #ifdef DEBUG
  Serial.println("stopListening");
  #endif
  radio.stopListening();

  #ifdef DEBUG
  Serial.println("openWritingPipe");
  #endif
  radio.openWritingPipe(target);
  const char text1[] = "Msg brdc";
  // #ifdef DEBUG
  // Serial.println("radio.write");
  // #endif
  // radio.write(&text1, sizeof(text1));
  #ifdef DEBUG
  Serial.println("radio.write2");
  #endif
  radio.write(payload, sizeof(payload));
  #ifdef DEBUG
  Serial.print("Broadcast Msg sent ");
  #endif

  #ifdef DEBUG
  Serial.println("openReadingPipe");
  #endif
  radio.openReadingPipe(0, rxAddrBrdc);
  radio.openReadingPipe(1, rxAddr1);
  #ifdef DEBUG
  Serial.println("startListening");
  #endif
  radio.startListening();
}

void ReceiveMqttMsg()
{
  // if (radio.available())
  // {
  //   char text[32] = {0};
  //   radio.read(&text, sizeof(text));
  //   // #ifdef DEBUG
  //   Serial.println(text);
  //   // #endif
  // }
}
//////////////////////////////// MQTT PART END /////////////////////////////////

//////////////////////////////// BUTTTON PART //////////////////////////////////
EInputStatus CheckButton()
{
  static bool newSwitchUp   = false;
  static bool newSwitchDown = false;
  static bool lastSwitchUp   = false;
  static bool lastSwitchDown = false;
  EInputStatus ret = ENone;

  newSwitchUp   = !digitalRead(SWITCHUPPIN);
  newSwitchDown = !digitalRead(SWITCHDOWNPIN);

  if (newSwitchUp != lastSwitchUp)
  {
    if (newSwitchUp) StartButtonTimer();
    else if (ButtonTimer) ret = ESwitchUp;
  }

  if (newSwitchDown != lastSwitchDown)
  {
    if (newSwitchDown) StartButtonTimer();
    else if (ButtonTimer)
    {
      if (ESwitchUp == ret) ret = ESwitchBoth;
      else ret = ESwitchDown;
    }
  }

  if (ButtonTimer == 0)
  {
    if (newSwitchUp) ret = ESwitchUpLong;

    if (newSwitchDown)
    {
      if (ESwitchUpLong == ret) ret = ESwitchBothLong;
      else ret = ESwitchDownLong;
    }
  }

  lastSwitchUp = newSwitchUp;
  lastSwitchDown = newSwitchDown;

  return ret;
}

void StartButtonTimer()
{ ButtonTimer = ButtonTimeout; }

void StopButtonTimer()
{ ButtonTimer = 0; }

bool ProcessButtonTimer()
{
  if (ButtonTimer)
  {
    --ButtonTimer;
    if (ButtonTimer == 0) return true;
  }
  return false;
}
////////////////////////////// BUTTTON PART END ////////////////////////////////
