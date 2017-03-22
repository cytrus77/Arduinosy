#include <SPI.h>  
#include "RF24.h" 

RF24 myRadio (7, 8); 
byte myAddress[6] = "F";
int lostPackets = 0;


struct package
{
  byte source[6];
  int  id;
  char text[20];
};

typedef struct package Package;
Package data;

void setup() 
{
  Serial.begin(115200);
  delay(100);

  myRadio.begin(); 
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  delay(1000);
  
  myRadio.openReadingPipe(1, myAddress);
  myRadio.startListening();

  Serial.println("Started");
}

void loop()  
{
  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read( &data, sizeof(data) );
    }
    
    Serial.println("\nPackage received");
    Serial.print("Source: ");
    Serial.print(data.source[0]);
    Serial.print(data.source[1]);
    Serial.print(data.source[2]);
    Serial.print(data.source[3]);
    Serial.print(data.source[4]);
    Serial.print(data.source[5]);
    Serial.print(" Id: ");
    Serial.print(data.id);
    Serial.print(" Txt: ");
    Serial.println(data.text);

    static int lastId = data.id;

    if (data.id - lastId > 1)
    {
      ++lostPackets;
    }
    lastId = data.id;
    Serial.print("Lost packets: ");
    Serial.println(lostPackets);
    
    sendResponse(data.source, data);
  }
}

void sendResponse(byte address[6], Package& data)
{
  myRadio.stopListening();
  myRadio.openWritingPipe(address);
//  delay(10);
  strncpy(data.source, myAddress, 6);
  char text[20] = "Ping response";
  strncpy(data.text, text, 20);
  myRadio.write(&data, sizeof(data)); 
  Serial.println("\nResponse sent");
  Serial.print("Source: ");
  Serial.print(data.source[0]);
  Serial.print(data.source[1]);
  Serial.print(data.source[2]);
  Serial.print(data.source[3]);
  Serial.print(data.source[4]);
  Serial.print(data.source[5]);
  Serial.print(" Id: ");
  Serial.print(data.id);
  Serial.print(" Txt: ");
  Serial.println(data.text);
  myRadio.openReadingPipe(1, myAddress);
  myRadio.startListening();
}

