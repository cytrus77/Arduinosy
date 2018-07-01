const int sensorPin = A4;    // select the input pin for the potentiometer
const int relayPin = 2;      // select the pin for the LED
int trigger = 400;  // variable to store the value coming from the sensor

void setup()
{
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.println("Started");
}

void loop()
{
  // read the value from the sensor:
  int sensorValue = analogRead(sensorPin);
  Serial.print("sensor value=");
  Serial.println(sensorValue);
  if (sensorValue > trigger)
  {
    digitalWrite(relayPin, LOW);
    delay(3000);
  }
  digitalWrite(relayPin, HIGH);
}
