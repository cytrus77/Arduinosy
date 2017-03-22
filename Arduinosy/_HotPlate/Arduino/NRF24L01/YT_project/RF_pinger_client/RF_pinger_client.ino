#include <SPI.h>
#include "RF24.h"

const int LED_PIN = 5;
RF24 myRadio (7, 8);
//byte myAddress[6] = "1";   // UNO
byte myAddress[6] = "2";   // nano
byte serverAddress[6] = "F";
struct package
{
  byte source[6];
  int  id = 1;
  char text[20] = "Ping request";
};

typedef struct package Package;
Package dataTx;
Package dataRx;
long lastTime = 0;
long lastMsgTime = 0;

void setup()
{ 
  strncpy(dataTx.source, myAddress, 6);
  Serial.begin(115200);
  delay(100);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  delay(100);

  pinMode(LED_PIN, OUTPUT);

  Serial.println("Started");
}

void loop()
{
  long timeNow = millis();
  if (timeNow - lastTime > 1000)
  {
    lastTime = timeNow;
    sendRequest(serverAddress, dataTx);
  }

  if (timeNow - lastMsgTime > 1500)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Timeout - shut off LED");
  }

  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read( &dataRx, sizeof(dataRx) );
    }
    
    Serial.println("\nPackage received");
    Serial.print("Source: ");
    Serial.print(dataRx.source[0]);
    Serial.print(dataRx.source[1]);
    Serial.print(dataRx.source[2]);
    Serial.print(dataRx.source[3]);
    Serial.print(dataRx.source[4]);
    Serial.print(dataRx.source[5]);
    Serial.print(" Id: ");
    Serial.print(dataRx.id);
    Serial.print(" Txt: ");
    Serial.println(dataRx.text);

    lastMsgTime = timeNow;
    digitalWrite(LED_PIN, LOW);
  }
}

void sendRequest(byte address[6], Package& data)
{
  myRadio.stopListening();
  myRadio.openWritingPipe(address);
  delay(10);
  myRadio.write(&data, sizeof(data)); 
  myRadio.openReadingPipe(1, myAddress);
  myRadio.startListening();
  Serial.println("\nRequest sent");
  Serial.print("Source: ");
  Serial.print(data.source[0]);
  Serial.print(data.source[1]);
  Serial.print(data.source[2]);
  Serial.print(data.source[3]);
  Serial.print(data.source[4]);
  Serial.print(data.source[5]);
  Serial.print(" Id: ");
  Serial.print(data.id++);
  Serial.print(" Txt: ");
  Serial.println(data.text);
}

