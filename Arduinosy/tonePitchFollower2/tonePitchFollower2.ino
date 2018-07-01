/*
  Pitch follower

 Plays a pitch that changes based on a changing analog input

 circuit:
 * 8-ohm speaker on digital pin 9
 * photoresistor on analog 0 to 5V
 * 4.7K resistor on analog 0 to ground

 created 21 Jan 2010
 modified 31 May 2012
 by Tom Igoe, with suggestion from Michael Flynn

This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/Tone2

 */


void setup() {
  // initialize serial communications (for debugging only):
  Serial.begin(9600);
}

void loop(){
  static unsigned int freq = 0;
  static const unsigned int period = 500;
  static unsigned long lastTimer = millis();
  const unsigned int freq1 = 3000;
  const unsigned int freq2 = 4000;

  if (millis() - lastTimer > period)
  {
    freq = freq == freq1 ? freq2 : freq1;
    lastTimer = millis();
    Serial.println(freq);
  }
  
  // play the pitch:
  tone(9, freq, 10);
}






