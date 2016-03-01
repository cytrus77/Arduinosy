#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#define DEBUG 1

#ifdef DEBUG
//SoftwareSerial debugSerial(DEBUGRXPIN, DEBUGTXPIN); // RX, TX
HardwareSerial& debugSerial = Serial;
#endif

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);

  #ifdef DEBUG
  debugSerial.begin(9600);
  debugSerial.println("setup()");
  #endif
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  wdt_reset();
//  Serial.println("loop");
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  static int count = 0;
  Serial.println(count++);
}



