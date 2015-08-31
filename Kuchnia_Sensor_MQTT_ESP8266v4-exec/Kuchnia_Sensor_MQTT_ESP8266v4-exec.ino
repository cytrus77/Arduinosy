#include <dht11.h>
#include <string.h>
#include "TimerOne.h"
#include "Kuchnia-defines.h"
#include "Kuchnia-objects.h"
#include "Czujnik.h"
#include "Utilities.h"


//#define DEBUG 1
//#define LOG 1


/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);

  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  

  static int counter = 0;
  if(!(loopCounter%measurePeriod))
  {
      counter = (counter+1)%sensor::m_sensorCounter;
      sensor::sensorPtr[counter]->getValue();
      sendUptime();
  }

//  #ifdef DEBUG
//  debugPort.println("!!!!!!!!!!!!!!!!MAIN LOOP!!!!!!!!!!!!!!!!!!!");
//  #endif
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  loopCounter++;
 
  if(pirLastValue != motionSensor.m_value)
  {
    if(motionSensor.m_value && ledDimmer.m_trigger >= photoSensor.m_value)
    {
      ledDimmer.setValue(100);
      pirLastValue = motionSensor.m_value;
    }
    else if (!motionSensor.m_value)
    {
      pirLastValue = motionSensor.m_value;
    }
  }
  ledDimmer.setDimmer();
}

