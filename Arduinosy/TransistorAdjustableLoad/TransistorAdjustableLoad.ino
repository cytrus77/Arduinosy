const int pwmPin = 9;

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  pinMode(pwmPin, OUTPUT);   // digital sensor is on digital pin 2
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) 
  {
    // get incoming byte:
    const int inByte = Serial.parseInt();
    analogWrite(pwmPin, inByte);
    Serial.println(inByte);
    
//    // read first analog input, divide by 4 to make the range 0-255:
//    firstSensor = analogRead(A0) / 4;
//    // delay 10ms to let the ADC recover:
//    delay(10);
//    // read second analog input, divide by 4 to make the range 0-255:
//    secondSensor = analogRead(1) / 4;
//    // read  switch, map it to 0 or 255L
//    thirdSensor = map(digitalRead(2), 0, 1, 0, 255);
//    // send sensor values:
//    Serial.write(firstSensor);
//    Serial.write(secondSensor);
//    Serial.write(thirdSensor);
  }
}
