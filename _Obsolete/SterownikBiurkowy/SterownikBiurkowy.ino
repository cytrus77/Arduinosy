#include <LiquidCrystal.h>
//From bildr article: http://bildr.org/2012/08/rotary-encoder-arduino/


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

enum rotary{
  left,
  center,
  right
};

enum menuValue
{
  swiatloMale,
  swiatloDuze
};

enum submenuValue
{
  moc,
  czasSwiecenia
};

long Light_Time = 120*1000;   //czas swiecenia sie swiatla w milisekundach
long PIR_Monitor_Time = 600*1000;   //czas sprawdzania PIR w milisekundach

//these pins can not be changed 2/3 are special pins
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderSwitchPin = A2; //push button switch
int pwmPin = 10;   // PWM dla swiatelek podmonitorowych
int pwm2Pin = 11;  // PWM dla swiatla duzego biurkowego
int pirPin = A0;
//int fotoSensorPin = A1;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

volatile int lastEncoded = 0;
volatile void * encoderValue;
volatile void * encoderMax;
volatile void * encoderMin;

int lastMSB = 0;
int lastLSB = 0;

//unsigned long lightTime = 60000; // w ms
unsigned long time;
double ledSwitch;

void setup() {
  Serial.begin (9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Wystartowalem"); 

  pinMode(encoderPin1, INPUT);
  pinMode(encoderPin2, INPUT);

  pinMode(encoderSwitchPin, INPUT);
  pinMode(pwmPin, OUTPUT);
  pinMode(pwm2Pin, OUTPUT);
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
  //  Serial.println("Dont klink");
    delay(50);
  } 
  else 
  {
    //button is being pushed
    if(ledSwitch > 0.01){
  //   ledSwitch = 0;
    }
    else
    {
      ledSwitch = 1;
      time = millis();
    }
    #ifdef DEBUG
    Serial.println("Klik");
    #endif
    delay(50);
  }

  pirTimer ();
  analogWrite(pwmPin, (int)(encoderValue*encoderValue*ledSwitch)/40);
  Serial.println(encoderValue);
  Serial.println((int)(encoderValue*encoderValue*ledSwitch)/40);
  char menu[10] = "Swiatelko";
  char submenu[4] = "Moc";
  LCDDispaly(3, menu, submenu, &encoderValue);
}


void LCDDispaly(int pos, String menu, String submenu, volatile long* value)
{
  static char txt_temp[16];
  static long value_temp;
  if(value_temp != *value)
  {
  lcd.clear();
  if(pos==1) { 
    lcd.setCursor(0, 0);
    lcd.print((char)0x7E);
  } 
  else if(pos==2) {
    lcd.setCursor(0, 1);
    lcd.print((char)0x7E);
  }
  else if(pos==3) {
    lcd.setCursor(11, 1);
    lcd.print((char)0x7E);
  }
  
  lcd.setCursor(1, 0);
  lcd.print(menu);
  lcd.setCursor(1, 1);
  lcd.print(submenu);
  lcd.setCursor(12, 1);
  lcd.print(*value);
  value_temp = *value;
  }
}


void updateEncoder() {
  static rotary encoderPosition = center;
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if ((sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) && &encoderValue > 0 ) 
  {
    if(encoderPosition == right) 
    {
      &encoderValue --;
      encoderPosition = center;
    }
    else if(encoderPosition == center) encoderPosition = right;
    else if(encoderPosition == left) encoderPosition = center;
  }

  if ((sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) && &encoderValue < 100 ) 
  {
    if(encoderPosition == left) 
    {
      &encoderValue ++;
      encoderPosition = center;
    }
    else if(encoderPosition == center) encoderPosition = left;
    else if(encoderPosition == right) encoderPosition = center;
  }
  lastEncoded = encoded; //store this value for next time
}


void pirTimer () {
  if (millis() - time > Light_Time) {
    if (ledSwitch > 0.01) ledSwitch = ledSwitch - 0.01;
    LCDDispaly(0, "Usypianie...", "", 0);
  }
  else if (millis() - time > PIR_Monitor_Time)
  {
    if (ledSwitch < 1) ledSwitch = ledSwitch + 0.01;
    LCDDispaly(0, "Spie sobie", "", 0);
  }

  if (digitalRead(pirPin)) {
    #ifdef DEBUG
    Serial.println("Wykryto ruch");
    #endif
    if (ledSwitch > 0.01) ledSwitch = 1;
    time = millis();
  }
}


