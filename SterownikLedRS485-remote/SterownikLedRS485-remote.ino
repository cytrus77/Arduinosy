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
#define SSerialRX         9  //Serial Receive pin
#define SSerialTX        10  //Serial Transmit pin

#define SSerialTxControl  8  //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define PwmLed1Pin        6
#define PwmLed2Pin        7

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;
int dimmingSpeed;
int Led1Pwm;
int Led2Pwm;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  
  pinMode(PwmLed1Pin, OUTPUT);
  pinMode(PwmLed2Pin, OUTPUT);
  pinMode(SSerialTxControl, OUTPUT);  
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  
  // Start the software serial port, to another device
  RS485Serial.begin(4800);   // set the data rate 
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  //Copy input data to output  
  if (RS485Serial.available()) 
  {
    byteSend = RS485Serial.read();   // Read the byte 
    
    digitalWrite(PwmLed1Pin, HIGH);  // Show activity
    delay(10);              
    digitalWrite(PwmLed1Pin, LOW);   
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit    
    RS485Serial.write(byteSend); // Send the byte back
    delay(10);   
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit      
//    delay(100);
  }// End If RS485SerialAvailable
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
//NONE

//*********( THE END )***********

