#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);

const byte rxAddrBrdc[6] = "00000";
//const byte rxAddr[6] = "00001";
const byte rxAddr[6] = "00002";

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  
  radio.begin();
  
  radio.openReadingPipe(0, rxAddrBrdc);
  radio.openReadingPipe(0, rxAddr);
  
  radio.startListening();

  Serial.println("ARDUINO started");
}

void loop()
{
  if (radio.available())
  {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    
    Serial.println(text);
  }
}
