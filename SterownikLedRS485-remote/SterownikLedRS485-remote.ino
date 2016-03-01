/* YourDuino SoftwareSerialExample1Remote
   - Used with YD_SoftwareSerialExampleRS485_1 on another Arduino
   - Remote: Receive data, loop it back...
   - Connect this unit Pins 10, 11, Gnd
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Pin 3 used for RS485 direction control   
   - Pin 13 LED blinks when data is received  
   
   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX         1  //Serial Receive pin
#define SSerialTX         3  //Serial Transmit pin
#define SSerialTxControl  2  //RS485 Direction control
#define StatusLedPin      0
#define PwmLed1Pin        7
#define PwmLed2Pin        A2

#define RS485Transmit    HIGH
#define RS485Receive     LOW

// status led states
#define Off              0
#define Light            1
#define Flash            2

// receiver states
#define StartBit1        0
#define StartBit2        1
#define PayloadSize      2
#define PayloadId        3
#define PayloadValue     4
#define CRC              5
#define EndBit1          6
#define EndBit2          7

// parameter IDs
#define dimmerSpeedID    10
#define led1PwmID        11
#define led2PwmID        12

#define RS485Timeout     50  // 50ms

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived = 0;
int CRCvalue = 0;
int receiveState = StartBit1;
int payloadSize = 0;
int* parameter;
bool CRCcheck = false;

int dimmingSpeed = 100;
int Led1Pwm = 0;
int Led2Pwm = 0;

// Value ready to set on hardware
int DimmerSpeedToSet = 100;
int Led1PwmToSet     = 0;
int Led2PwmToSet     = 0;

unsigned long rs485timer = 0;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  pinMode(StatusLedPin, OUTPUT);
  pinMode(PwmLed1Pin, OUTPUT);
  pinMode(PwmLed2Pin, OUTPUT);
  pinMode(SSerialTxControl, OUTPUT);  

  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver

  // Start the software serial port, to another device
  RS485Serial.begin(4800);   // set the data rate 
  setStatusLed(Light);
}//--(end setup )---

int i = 0;
void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{ 
  while (RS485Serial.available()) 
  {
    byteReceived = RS485Serial.read();   // Read the byte 
      
    switch(receiveState)
    {
      case StartBit1:
        if (byteReceived == (uint8_t)'H')
        {
          rs485resetTimer();
          setStatusLed(Flash);
          receiveState = StartBit2;
        }
        break;
      case StartBit2:
        if (byteReceived == (uint8_t)'I')
        {
          receiveState = PayloadSize;
          CRCvalue = 0;
        }
        else
          receiveState = StartBit1;
        break;
      case PayloadSize:
        if (byteReceived < 10)
        {
          payloadSize = byteReceived;
          receiveState = PayloadId;
        }
        else receiveState = StartBit1;
        break;
      case PayloadId:
        if (byteReceived == dimmerSpeedID)
          parameter = &dimmingSpeed;
        else if (byteReceived == led1PwmID)
          parameter = &Led1Pwm;
        else if (byteReceived == led2PwmID)
          parameter = &Led2Pwm;
        CRCvalue += byteReceived;
        receiveState = PayloadValue;
        break;
      case PayloadValue:
        *parameter = byteReceived;
        if (--payloadSize)
          receiveState = PayloadId;
        else
          receiveState = CRC;
        CRCvalue += byteReceived;
        break;
      case CRC:
       // if ((int)CRCvalue == (int)byteReceived)
          CRCcheck = true;
       // else
       //   CRCcheck = false;
        CRCvalue = 0;
        receiveState = EndBit1;
        break;
      case EndBit1:
        if (byteReceived == 'B')
          receiveState = EndBit2;
        else if (byteReceived == 'H')
          receiveState = StartBit2;
        break;
      case EndBit2:
        if (byteReceived == 'Y')
          receiveState = StartBit1;
        else if (byteReceived == 'H')
          receiveState = StartBit2;
        break;
    }
  }// End If RS485SerialAvailable

  setReveivedValues();
  checkIfRs485expired();
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
//status led control
void setStatusLed(int status)
{
  static bool lastState = LOW;
  static unsigned long lastChangeTime = 0;
  
  switch(status)
  {
    case Off:
      digitalWrite(StatusLedPin, LOW);
      break;
    case Light:
      digitalWrite(StatusLedPin, HIGH);
      break;
    case Flash:
      if (millis() - lastChangeTime > 100)
      {
        digitalWrite(StatusLedPin, lastState = !lastState);
        lastChangeTime = millis();
      }
      break;
    default:
      break;
  }
}

void setReveivedValues()
{
  if (CRCcheck)
  {
    Led1PwmToSet = Led1Pwm;
    Led2PwmToSet = Led2Pwm;
    DimmerSpeedToSet = dimmingSpeed;
    
    analogWrite(PwmLed1Pin, Led1PwmToSet);
    analogWrite(PwmLed2Pin, Led2PwmToSet);

    CRCcheck = false;
  }
}

void rs485resetTimer()
{
  rs485timer = millis();
}

void checkIfRs485expired()
{
  if (millis() - rs485timer > RS485Timeout)
    receiveState = StartBit1;
}

//*********( THE END )***********

