#include <LiquidCrystal.h>
//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/

#define Light_Time 120   //czas swiecenia sie swiatla w sekundach

/*
  The circuit:
 * LCD RS pin to digital pin 9
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 7
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 5
 * LCD D7 pin to digital pin 4
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderSwitchPin = 4; //push button switch
int pwmPin = 0;
int pirPin = A0;
int fotoSensorPin = A1;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

unsigned long lightTime = 60000; // w ms
unsigned long time;
double ledSwitch;

void setup() {
  Serial.begin (9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Uruchamianie!"); 

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  pinMode(encoderSwitchPin, INPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(pirPin, INPUT);

  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  digitalWrite(encoderSwitchPin, HIGH); //turn pullup resistor on


  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3)
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  ledSwitch = 1;
  time = millis();
}

void loop() {
  //Do stuff here
  if (digitalRead(encoderSwitchPin)) {
    //button is not being pushed
  } else {
    //button is being pushed
    if(ledSwitch > 0.01){
      ledSwitch = 0;
    }
    else
    {
      ledSwitch = 1;
      time = millis();
    }
    Serial.println("Klik");
    delay(200);
  }

  pirTimer ();
  analogWrite(pwmPin, encoderValue*encoderValue*ledSwitch);
  char menu[10] = "Swiatelko";
  LCDDispaly(menu, &encoderValue);
  
//Test code
/*
  Serial.println("Light sensor:");
  Serial.println(analogRead(fotoSensorPin));
  Serial.println("PWM:");
  Serial.println(log((double)encoderValue + 1.0) * 55.372);
  //Serial.println("Encoder value:");
  //Serial.println(encoderValue);
  Serial.println("Led switch:");
  Serial.println(ledSwitch);
*/  
  delay(10); //just here to slow down the output, and show it will work  even during a delay

}

void LCDDispaly(char* menu, volatile long* value)
{
  lcd.setCursor(0, 0);
  lcd.print(*menu);
  lcd.setCursor(0, 1);
  lcd.print(*value);
}

void updateEncoder() {
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if ((sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) && encoderValue < 100) encoderValue ++;
  if ((sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) && encoderValue > 0) encoderValue --;

  lastEncoded = encoded; //store this value for next time
}

void pirTimer () {
  if (millis() - time > lightTime) {
    if (ledSwitch > 0.01) ledSwitch = ledSwitch - 0.01;
  }
  else
  {
    Serial.println("OFF Timer:");
    Serial.println(millis() - time);
  }

  if (digitalRead(pirPin)) {
    Serial.println("Wykryto ruch");
    if (ledSwitch > 0.01) ledSwitch = 1;
    time = millis();
  }
}

