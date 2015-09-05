
#include <Wire.h>

void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(19200);  // start serial for output
}
int x;
void loop()
{
  Wire.requestFrom(2, 4);    // request 6 bytes from slave device #2

  while(Wire.available())    // slave may send less than requested
  { 
    x = Wire.read(); // receive a byte as character
    Serial.println(x);         // print the character
  }

  delay(500);
}

