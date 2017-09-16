const int ADC_PIN    = A0;
const int BUZZER_PIN = 3;
const int LED_PIN    = 9;

int minVoltage = 255;
int maxVoltage = 0;

void setup()
{
  Serial.begin(9600);
  
  analogReference(INTERNAL);
  pinMode(ADC_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  const int voltage = analogRead(ADC_PIN);
  minVoltage = voltage;
  maxVoltage = voltage + 100;

  Serial.println("Setup done.");
}

void loop()
{
  const int voltage = analogRead(ADC_PIN);
  Serial.print("ADC=");
  Serial.println(voltage);
  updateLimits(voltage);

  if (isVoltageHigh(voltage))
  {
    Serial.println("BUZZER triger");
    playSong();
    blinkLed();
  }
}

void updateLimits(const int voltage)
{
  if (minVoltage > voltage) minVoltage = voltage;
  if (maxVoltage < voltage) maxVoltage = voltage;
}

bool isVoltageHigh(const int voltage)
{
  int absFromMin = voltage - minVoltage;
  int absFromMax = maxVoltage - voltage;

  return (absFromMax < absFromMin);
}

void playSong()
{
  const int buzzer = BUZZER_PIN;
  const int loops = 4;
  for (int i = 0; i < loops; ++i)
  {
    digitalWrite(buzzer, HIGH);
    delay(800);
    digitalWrite(buzzer, LOW);
    delay(200);
//    tone(buzzer, 1000); // Send 1KHz sound signal...
//    delay(500);         // ...for 0.5 sec
//    tone(buzzer, 2000); // Send 2KHz sound signal...
//    delay(500);         // ...for 0.5 sec
  }
  noTone(buzzer);     // Stop sound...
}

void blinkLed()
{
  const int led = LED_PIN;
  const int loops = 1;
  for (int i = 0; i < loops; ++i)
  {
    digitalWrite(led, HIGH);
    delay(1000);         // ...for 1 sec
    digitalWrite(led, LOW);
    delay(300);         // ...for 0.3 sec
  }
}

