#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);

const byte rxAddrBrdc[6] = "00000";
const byte rxAddr1[6] = "00001";
const byte rxAddr2[6] = "00002";

void setup()
{
  while (!Serial);
  Serial.begin(9600);
  
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(rxAddrBrdc);
  radio.stopListening();

  Serial.println("ARDUINO started");
}

int counter = 0;

void loop()
{
  radio.openWritingPipe(rxAddrBrdc);
  const char text1[] = "Msg brdc";
  radio.write(&text1, sizeof(text1));
  radio.write(counter, sizeof(counter));
  Serial.print("Broadcast Msg sent ");
  Serial.println(counter++);
  delay(1000);

  radio.openWritingPipe(rxAddr1);
  const char text2[] = "Msg rx1";
  radio.write(&text2, sizeof(text2));
  radio.write(counter, sizeof(counter));
  Serial.print("Msg sent to 1");
  Serial.println(counter++);
  delay(1000);

  radio.openWritingPipe(rxAddr2);
  const char text3[] = "Msg rx2";
  radio.write(&text3, sizeof(text3));
  radio.write(counter, sizeof(counter));
  Serial.print("Msg sent to 2");
  Serial.println(counter++);
  delay(1000);
}
