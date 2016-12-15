#include <SPI.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Defines.h"


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}

void loop()
{
  Serial.println("123456");
  delay(1000);
}
