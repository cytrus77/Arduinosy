#include <SPI.h>  
#include "RF24.h"

RF24 myRadio (7, 8);
byte addresses[][6] = {"0", "9"};

struct package
{
  int id=1;
  float temperature = 18.3;
  char  text[100] = "Text to be transmitted";
};

typedef struct package Package;
Package data;
Package data2;

void setup()
{
  data2.text[0] = '1';
  data2.text[1] = '2';
  
  Serial.begin(115200);
  delay(1000);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  delay(1000);

  Serial.println("Started");
}

void loop()
{
  myRadio.openWritingPipe( addresses[0]);
  myRadio.write(&data, sizeof(data)); 

  Serial.print("\nPackage:");
  Serial.print(data.id);
  Serial.print("\n");
  Serial.println(data.temperature);
  Serial.println(data.text);
  data.id = data.id + 1;
  data.temperature = data.temperature+0.1;
  delay(1000);

  myRadio.openWritingPipe( addresses[1]);
  myRadio.write(&data2, sizeof(data2)); 

  Serial.print("\nPackage BRC:");
  Serial.print(data2.id);
  Serial.print("\n");
  Serial.println(data2.temperature);
  Serial.println(data2.text);
  data2.id = data2.id + 1;
  data2.temperature = data2.temperature+0.1;
  delay(1000);

}
