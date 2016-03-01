#include <SoftwareSerial.h>

#define SerialTransmit      12   //RS485 управляющий контакт на arduino pin 10
#define SerialReceive       11
#define RS485rxPin          2
#define RS485txPin          3
#define RS485TransmitOn     HIGH
#define RS485TransmitOff    LOW  
#define RS485ReceiveOn      LOW
#define RS485ReceiveOff     HIGH  

SoftwareSerial mySerial(RS485rxPin, RS485txPin); // RX, TX
int buffer[10];
int bufferIt = 0;

void setup(void) 
{
  Serial.begin(9600, SERIAL_8N1);
  mySerial.begin(4800);
  pinMode(SerialTransmit, OUTPUT);  
  pinMode(SerialReceive, OUTPUT);
  pinMode(RS485rxPin, INPUT);  
  pinMode(RS485txPin, OUTPUT); 
  
  digitalWrite(SerialTransmit, RS485TransmitOff); 
  digitalWrite(SerialReceive, RS485ReceiveOff); 
  Serial.println("START");  
  delay(100); 
}

uint8_t pwmLight = 0;
void loop(void) 
{
    Serial.println("Pomiar");
    digitalWrite(SerialTransmit, RS485TransmitOn); 
    digitalWrite(SerialReceive, RS485ReceiveOff); 
    mySerial.write((uint8_t)'H');
    mySerial.write((uint8_t)'I');
    mySerial.write((uint8_t)0x1);
    mySerial.write((uint8_t)0xB); 
    mySerial.write((uint8_t)pwmLight);
    mySerial.write((uint8_t)(0xB) + pwmLight);
    mySerial.write((uint8_t)'B');
    mySerial.write((uint8_t)'Y');
    bufferIt = 0;

    ++pwmLight;

}

bool doTheJob()
{
  bool result = false;
  const int period = 30;  //in ms - 1s
  static long lastMillis = 0;
  long currentMillis = millis();
  
  if (currentMillis - lastMillis > period)
  {
    lastMillis = currentMillis;
    result = true;
  } 
  
  return result;
}


