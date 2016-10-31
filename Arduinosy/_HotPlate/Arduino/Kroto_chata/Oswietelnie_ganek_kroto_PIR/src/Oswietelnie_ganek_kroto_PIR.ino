// ---------------------------------------------------------------------------
// This example shows how to use NewPing's ping_timer method which uses the Timer2 interrupt to get the
// ping time. The advantage of using this method over the standard ping method is that it permits a more
// event-driven sketch which allows you to appear to do two things at once. An example would be to ping
// an ultrasonic sensor for a possible collision while at the same time navigating. This allows a
// properly developed sketch to multitask. Be aware that because the ping_timer method uses Timer2,
// other features or libraries that also use Timer2 would be effected. For example, the PWM function on
// pins 3 & 11 on Arduino Uno (pins 9 and 11 on Arduino Mega) and the Tone library. Note, only the PWM
// functionality of the pins is lost (as they use Timer2 to do PWM), the pins are still available to use.
// NOTE: For Teensy/Leonardo (ATmega32U4) the library uses Timer4 instead of Timer2.
// ---------------------------------------------------------------------------
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Dimmer.h"
#include "DimmerPir.h"
#include "MqttSensor.h"

#define TIMER0PERIOD   3000 // 3ms
#define LED_TIMEOUT    30 // in seconds

#define LED_PIN        6
#define PIR_PIN        3
#define LIGHT_PIN      A5

dimmer ledDimmer(LED_PIN, 0);
mqttSensor lightSensor(0, 0, LIGHT_PIN, ANALOGTYPE, INVERTEDSCALE, 0);
mqttSensor pirSensor(0, 0, PIR_PIN, DIGITALTYPE, NORMALSCALE, 0);
dimmerPir ledDimmerPir(0, 0, &ledDimmer, &pirSensor, &lightSensor);

////////////////////////////// SETUP ////////////////////////////////////////////
void setup()
{
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  wdt_enable(WDTO_8S);
  Serial.println("ARDUINO: startup");

  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);

  ledDimmerPir.setLightTrigger(50);
  ledDimmerPir.setPirFlag(true);
  ledDimmer.setTimeout(LED_TIMEOUT * 1000000 / TIMER0PERIOD);
}

//////////////////////////// MAIN LOOP //////////////////////////////////////////

void loop()
{
  wdt_reset();

  ledDimmerPir.checkSensors();
}

void TimerLoop()
{
  ledDimmer.setDimmer();
  ledDimmer.processTimer();
}
