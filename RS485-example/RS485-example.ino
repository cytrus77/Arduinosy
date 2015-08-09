/* YourDuino SoftwareSerialExample1
   - Connect to another Arduino running "YD_SoftwareSerialExampleRS485_1Remote"
   - Connect this unit Pins 10, 11, Gnd
   - Pin 3 used for RS485 direction control
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Open Serial Monitor, type in top window. 
   - Should see same characters echoed back from remote Arduino

   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin

#define SSerialTxControl 3   //RS485 Direction control

#define RS485Transmit    HIGH
#define RS485Receive     LOW

#define Pin13LED         13

/*-----( Declare objects )-----*/
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX

/*-----( Declare Variables )-----*/
unsigned long byteReceived;
unsigned long byteSend1, byteSend2;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial.println("YourDuino.com SoftwareSerial remote loop example");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");
  
  pinMode(Pin13LED, OUTPUT);   
  pinMode(SSerialTxControl, OUTPUT);    
  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver   
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
 // digitalWrite(Pin13LED, HIGH);  // Show activity
  //if (Serial.available())
  {
    //Serial.println("Wysylam zapytanie");
    word byteSend1 = 0x0203;
    word byteSend2 = 0x0000;
    word byteSend3 = 0x0001;
    word byteSend4 = 0x8439;//Serial.read();
    
    digitalWrite(SSerialTxControl, RS485Transmit);  // Enable RS485 Transmit   
    Serial.write(byteSend1);          // Send byte to Remote Arduino
    Serial.write(byteSend2); 
    Serial.write(byteSend3); 
    Serial.write(byteSend4);
    
    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(10);
    digitalWrite(SSerialTxControl, RS485Receive);  // Disable RS485 Transmit       
  }
  
  if (Serial.available())  //Look for data from other Arduino
   {
     int flaga = 0;
     for(int i=0; i < 50; i++)
     {
        //Serial.println("Odbieram dane");
        digitalWrite(Pin13LED, HIGH);  // Show activity
        byteReceived = Serial.read();    // Read received byte
        Serial.println(byteReceived, HEX);        // Show on Serial Monitor
        if(byteReceived == 0x02 && flaga == 0) flaga = 1;
        else if( (byteReceived == 0x02 || byteReceived == 0x03) && flaga != 0)
        {
           Serial.println("MAMY TO!!!!!!!!!!!!!!!!!!!!!!!!!!");
        }
        else {
           flaga = 0;
        }
        delay(10);
        digitalWrite(Pin13LED, LOW);  // Show activity   
     }
   }
   delay(1000);

}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********

