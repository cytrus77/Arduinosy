#include <Sleep_n0m1.h>

Sleep sleep;

#define intPin 2

void setup()
{
   Serial.begin(9600);

   Serial.println("Arduino started");
}

int counter = 0;

void loop()
{
  ++counter;
  delay(100); ////delays are just for serial print, without serial they can be removed
  Serial.print("execute your code here; count = ");
  Serial.println(counter);

  Serial.println("Sleeping Till Interrupt");

  delay(100); //delay to allow serial to fully print before sleep
  //Sleep till interrupt pin equals a particular state.
  //In this case "low" is state 0.
  sleep.sleepPinInterrupt(intPin,LOW); //(interrupt Pin Number, interrupt State)

delay(100);

  sleep.pwrDownMode(); //set sleep mode
}

// #include <avr/wdt.h>
// #include <avr/sleep.h>
// #include "TimerOne.h"
// #include "Dimmer.h"
//
// const unsigned int VBAT_LIMIT = 2900; // in miliVolts
// const unsigned long TIMER0PERIOD = 10000;  // 10ms
// const unsigned long CYCKES_IN_SECOND = 1000000 / TIMER0PERIOD;
// const unsigned int LED_TIMEOUT   = 120;    // in seconds
// const unsigned int NIGHT_DIMMER_LEVEL = 30;  // set LED dimmer to 30% when night comes
// const unsigned int VBAT_CHECK_COUNTER = 5 * CYCKES_IN_SECOND; // 5seconds
//
// const byte LED_PIN = 6;
// const byte PIR_PIN = 2;   // interruptPin
// const byte LIGHT_PIN = 3; // interruptPin
// const byte VBAT_PIN  = A3;
//
// dimmer ledDimmer(LED_PIN, 0, 0, LED_TIMEOUT, CYCKES_IN_SECOND);
// bool night = false;
// unsigned int checkBatteryCounter = 0;
//
// ////////////////////////////// SETUP ////////////////////////////////////////////
// void setup()
// {
//   Serial.begin(19200);
//   Serial.println("ARDUINO: startup start");
//
//   wdt_enable(WDTO_2S);
//
//   pinMode(PIR_PIN, INPUT_PULLUP);
//   pinMode(LIGHT_PIN, INPUT_PULLUP);
//   pinMode(VBAT_PIN, INPUT);
//   night = digitalRead(LIGHT_PIN);
//
//   Timer1.initialize(TIMER0PERIOD);
//   Timer1.attachInterrupt(TimerLoop);
//
//   sleep_enable();          // enables the sleep bit in the mcucr register
//
//   attachInterrupt(digitalPinToInterrupt(PIR_PIN), pirChange, CHANGE);
//   attachInterrupt(digitalPinToInterrupt(LIGHT_PIN), lightChange, CHANGE);
//
//   Serial.println("ARDUINO: started");
// }
//
// //////////////////////////// MAIN LOOP //////////////////////////////////////////
// void loop()
// {
//   wdt_reset();
//
//   if (checkBatteryCounter > VBAT_CHECK_COUNTER)
//   {
//     checkBatteryCounter = 0;
//     checkIfBatteryOK();
//   }
// }
//
// void TimerLoop()
// {
//   ledDimmer.setDimmer();
//   ledDimmer.processTimer(NIGHT_DIMMER_LEVEL);
//   ++checkBatteryCounter;
// }
//
// void pirChange()
// {
//   if (night)
//   {
//     ledDimmer.setValue(100);
//     Serial.println("Set light to 100");
//   }
//
//   Serial.println("PIR detected");
// }
//
// void lightChange()
// {
//   night = digitalRead(LIGHT_PIN);
//
//   if (night)
//   {
//     ledDimmer.setValue(NIGHT_DIMMER_LEVEL);
//     Serial.print("Set light to ");
//     Serial.println(NIGHT_DIMMER_LEVEL);
//   }
//   else
//   {
//     ledDimmer.setValue(0);
//     Serial.println("Set light to 0");
//   }
//
//   Serial.print("Night = ");
//   Serial.println(night);
// }
//
// void checkIfBatteryOK()
// {
//   auto vbat = analogRead(VBAT_PIN) * 5;
//
//   Serial.print("V Battery [mV]= ");
//   Serial.println(vbat);
//
//   if (vbat < VBAT_LIMIT)
//   {
//     Serial.println("Go to sleep");
//     sleepNow();
//   }
// }
//
// void sleepNow()
// {
//   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//   cli();
// //  sleep_bod_disable();
//   sei();
//   delay(100);
//   sleep_cpu();
//   /* wake up here */
//   sleep_disable();
// }
