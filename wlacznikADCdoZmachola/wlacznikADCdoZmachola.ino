#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#define SILENTTIME 6000  // 1 unit = 10ms

bool powerSwitch = true;
long msTimer  = 0;
int adcValue  = 0;
int switchPin = 9;
int adcPin    = A0;

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  
  #ifdef DEBUG
  Serial.begin(19200);
  Serial.println("Start");
  #endif

  pinMode(switchPin, OUTPUT);
  digitalWrite(switchPin, HIGH);
  
  //Smooth dimming interrupt init
  Timer1.initialize(10000);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_2S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  wdt_reset();
  adcValue = analogRead(adcPin);
  #ifdef DEBUG
  Serial.print("adcValue = ");
  Serial.println(adcValue);
  #endif

  if (adcValue > 5)
  {
     powerSwitch = false;
     #ifdef DEBUG
     Serial.println("powerSwitch ON");
     #endif
     digitalWrite(switchPin, powerSwitch);
     msTimer = 0;
  }

  #ifdef DEBUG
  Serial.print("msTimer = ");
  Serial.println(msTimer);
  #endif

  if (!powerSwitch && SILENTTIME < msTimer)
  {
     powerSwitch = true;
     #ifdef DEBUG
     Serial.println("powerSwitch OFF");
     #endif
     digitalWrite(switchPin, powerSwitch);
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  if (!powerSwitch)
     msTimer++;
  else
     msTimer = 0;
}


