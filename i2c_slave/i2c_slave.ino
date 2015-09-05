// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Wire.begin(2);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
    Serial.begin(19200);  // start serial for output
}

void loop()
{
  delay(100);

}

byte x[4];

byte x2 = 0x6;
byte x3 = 0x9;
byte x4 = 0xD;

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  x[0] = 0x2;
x[1] = 0x5;
x[2] = 0xA; 
x[3] = 0xD;
  Wire.write(x[0]); // receive byte as a character
  Wire.write(x[1]);
  Wire.write(x[2]);
  Wire.write(x[3]);
}
