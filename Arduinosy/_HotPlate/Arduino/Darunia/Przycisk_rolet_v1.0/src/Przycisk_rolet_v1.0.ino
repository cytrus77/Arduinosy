#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Defines.h"

ERollerState rollerState = ERollerStop;
unsigned long Timer = 0;
unsigned long MqttTimer = 0;
const unsigned long RollerTimeout = ROLLERTIMEOUT * COUNTIN1SEC;
const unsigned long MqttTimeout = ROLLERTIMEOUT * COUNTIN1SEC;

void StartMqttTimer()
{
  MqttTimer = MqttTimeout;
}

void StopMqttTimer()
{
  MqttTimer = 0;
}

bool ProcessMqttTimer()
{
  if (MqttTimer)
  {
    --MqttTimer;

    if (MqttTimer == 0)
    {
      return true;
    }
  }

  return false;
}

void StartTimer()
{
  Timer = RollerTimeout;
}

void StopTimer()
{
  Timer = 0;
}

bool ProcessTimer()
{
  if (Timer)
  {
    --Timer;

    if (Timer == 0)
    {
      return true;
    }
  }

  return false;
}

void RollerUp()
{
  digitalWrite(ROLLERUPPIN, LOW);
  digitalWrite(ROLLERDOWNPIN, HIGH);
}

void RollerDown()
{
  digitalWrite(ROLLERUPPIN, HIGH);
  digitalWrite(ROLLERDOWNPIN, LOW);
}

void RollerStop()
{
  digitalWrite(ROLLERUPPIN, HIGH);
  digitalWrite(ROLLERDOWNPIN, HIGH);
}

EInputStatus CheckInputs()
{
  static bool mqttUp   = false;
  static bool mqttDown = false;
  static bool switchUp = false;
  static bool switchDown = false;
  bool switchUpUpdated = false;
  bool switchDownUpdated = false;

  bool newSwitchUp   = !digitalRead(SWITCHUPPIN);
  bool newSwitchDown = !digitalRead(SWITCHDOWNPIN);
  bool newMqttUp   = !digitalRead(MQTTROLLERUPPIN);
  bool newMqttDown = !digitalRead(MQTTROLLERDOWNPIN);

  if (newMqttUp != mqttUp)
  {
    StartMqttTimer();
    mqttUp = newMqttUp;
  }

  if (newMqttDown != mqttDown)
  {
    StartMqttTimer();
    mqttDown = newMqttDown;
  }

  if (newSwitchUp != switchUp)
  {
    switchUpUpdated = true;
    switchUp = newSwitchUp;
    StopMqttTimer();
  }

  if (newSwitchDown != switchDown)
  {
    switchDownUpdated = true;
    switchDown = newSwitchDown;
    StopMqttTimer();
  }

  if (switchUpUpdated || switchDownUpdated)
  {
    if (switchUp && switchDown)
      return ESwitchBoth;
    else if (switchUp)
      return ESwitchUp;
    else if (switchDown)
      return ESwitchDown;
  }
  else if (MqttTimer)
  {
    if (mqttUp && !mqttDown)
      return EMqttUp;
    else if (!mqttUp && mqttDown)
      return EMqttDown;
  }

  return ENone;
}

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);

  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_4S);

  pinMode(ROLLERUPPIN, OUTPUT);
  pinMode(ROLLERDOWNPIN, OUTPUT);
  digitalWrite(ROLLERUPPIN, HIGH);
  digitalWrite(ROLLERDOWNPIN, HIGH);
  pinMode(SWITCHUPPIN, INPUT_PULLUP);
  pinMode(SWITCHDOWNPIN, INPUT_PULLUP);
  pinMode(MQTTROLLERUPPIN, INPUT_PULLUP);
  pinMode(MQTTROLLERDOWNPIN, INPUT_PULLUP);

  #ifdef DEBUG
  debugPort.println("ARDUINO: system started");
  #endif

  delay(200);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  wdt_reset();

  EInputStatus input = CheckInputs();

  switch(input)
  {
    case ESwitchUp:
      if (ERollerStop == rollerState)
      {
        StartTimer();
        rollerState = ERollerUp;
      }
      else rollerState = ERollerStop;
    break;

    case ESwitchDown:
      if (ERollerStop == rollerState)
      {
        StartTimer();
        rollerState = ERollerDown;
      }
      else rollerState = ERollerStop;
    break;

    case ESwitchBoth:
      rollerState = ERollerStop;
    break;

    case EMqttUp:
      rollerState = ERollerUp;
    break;

    case EMqttDown:
      rollerState = ERollerDown;
    break;

    case ENone:
     if (Timer == 0)
     {
       rollerState = ERollerStop;
     }
    default:
    break;
  }

  switch(rollerState)
  {
    case ERollerStop:
    RollerStop();
    StopTimer();
    break;

    case ERollerUp:
    RollerUp();
    break;

    case ERollerDown:
    RollerDown();
    break;

    default:
    RollerStop();
    StopTimer();
    break;
  }

  Serial.print("input = ");
  Serial.println(input);
  Serial.print("rollerState = ");
  Serial.println(rollerState);
  Serial.print("Timer = ");
  Serial.println(Timer);
  delay(50);
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  if (ProcessTimer())
  {
    RollerStop();
    rollerState = ERollerStop;
  }

  ProcessMqttTimer();
}
