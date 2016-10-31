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
#include <NewPing.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Dimmer.h"
#include "DimmerPir.h"
#include "MqttSensor.h"

#define TIMER0PERIOD       3000 // 3ms

#define TRIGGER_PIN   12 // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN      11 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE  30 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define LED_TIMEOUT   300 // in seconds

#define LED_PIN        6
#define PIR_PIN        3
#define LIGHT_PIN      A5

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
dimmer ledDimmer(LED_PIN, 0);
mqttSensor lightSensor(0, 0, LIGHT_PIN, ANALOGTYPE, INVERTEDSCALE, 0);
mqttSensor pirSensor(0, 0, PIR_PIN, DIGITALTYPE, NORMALSCALE, 0);
dimmerPir ledDimmerPir(0, 0, &ledDimmer, &pirSensor, &lightSensor);

unsigned int pingSpeed = 100; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
unsigned int pingValue[10] = {0,0,0,0,0,0,0,0,0,0};
int pingPointer = 0;

////////////////////////////// SETUP ////////////////////////////////////////////
void setup()
{
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  wdt_enable(WDTO_8S);
  Serial.println("ARDUINO: startup");
  pingTimer = millis(); // Start now.

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

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

  // Notice how there's no delays in this sketch to allow you to do other processing in-line while doing distance pings.
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
  // Do other stuff here, really. Think of it as multi-tasking.
  unsigned int ledValue = getAvgPingValue();
  static unsigned int lastLedValue = 0xFFFF;
  if (abs(ledValue - lastLedValue) > 50)
  {
    if (ledValue < 200) ledValue = 200;
    if (ledValue > 700) ledValue = 700;
    ledValue = map(ledValue, 200, 700, 255, 0);
    ledValue = ledValue * ledValue / 255;
    ledDimmer.setValue(ledValue);
    lastLedValue = ledValue;
  }

  ledDimmerPir.checkSensors();

  // Serial.print("PIR: ");
  // Serial.println(digitalRead(PIR_PIN));
  // Serial.print("Light: ");
  // Serial.println(analogRead(LIGHT_PIN));
}

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    pingValue[pingPointer++] = sonar.ping_result;
    pingPointer = pingPointer%10;
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
  }
  // Don't do anything here!
}

unsigned int getAvgPingValue()
{
  unsigned long temp = 0;

  for (int i = 0; i < 10 ; ++i)
  {
    temp += pingValue[i];
  }

  return temp/10;
}

void TimerLoop()
{
  ledDimmer.setDimmer();
  ledDimmer.processTimer();
}
