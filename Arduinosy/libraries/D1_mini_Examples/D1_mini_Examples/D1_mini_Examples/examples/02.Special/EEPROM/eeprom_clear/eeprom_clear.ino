#include <Arduino.h>

/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0.
 *
 * This example code is in the public domain.
 * https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/examples/eeprom_clear/eeprom_clear.ino
 */

#include <EEPROM.h>

void setup()
{
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);

  // turn the LED on when we're done
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  EEPROM.end();
}

void loop()
{
}
