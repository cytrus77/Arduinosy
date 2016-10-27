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
  mySerial.begin(9600);
  pinMode(SerialTransmit, OUTPUT);  
  pinMode(SerialReceive, OUTPUT);
  pinMode(RS485rxPin, INPUT);  
  pinMode(RS485txPin, OUTPUT); 
  
  digitalWrite(SerialTransmit, RS485TransmitOff); 
  digitalWrite(SerialReceive, RS485ReceiveOff); 
  Serial.println("START");  
  delay(100); 
}


void loop(void) 
{
  if (doTheJob())
  {
    Serial.println("Pomiar");
    digitalWrite(SerialTransmit, RS485TransmitOn); 
    digitalWrite(SerialReceive, RS485ReceiveOff); 
    mySerial.write((uint8_t)0x02);
    mySerial.write((uint8_t)0x03);
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)0x00); 
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)0x01);
    mySerial.write((uint8_t)0x84);
    mySerial.write((uint8_t)0x39);
    bufferIt = 0;
  }
  else 
  {
    digitalWrite(SerialTransmit, RS485TransmitOff); 
    digitalWrite(SerialReceive, RS485ReceiveOn); 
    if(mySerial.available())
    {
      while( mySerial.available()) 
      {
        buffer[bufferIt] = mySerial.read();
        Serial.println(buffer[bufferIt]);
        ++bufferIt;
      }
      if (bufferIt >= 7)
      {
        int windSpeed = (buffer[3] * 0xFF + buffer[4]);
        Serial.print("Wind speed = ");
        Serial.println(windSpeed);
      }
    }
  }
}

bool doTheJob()
{
  bool result = false;
  const int period = 1000;  //in ms - 1s
  static long lastMillis = 0;
  long currentMillis = millis();
  
  if (currentMillis - lastMillis > period)
  {
    lastMillis = currentMillis;
    result = true;
  } 
  
  return result;
}


