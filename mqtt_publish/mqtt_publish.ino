#include <SPI.h>
#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>
#include <dht11.h>
#include <Ethernet.h>

#define DHT11PIN 3

dht11 DHT11;
NanodeMQTT mqtt(&uip);
signed int temperatura;
unsigned int wilgotnosc;


void setup() {
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.begin(38400);
  Serial.println("MQTT Publish test");
  
  unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
  uip.init(macaddr);

  // FIXME: use DHCP instead
  uip.set_ip_addr(192, 168, 137, 100);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
  Serial.println("Link is up");

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 137, 2);
  mqtt.connect();

  Serial.println("setup() done");
}




void loop() {
  uip.poll();
  static int i = 0;
  DHT11Pomiar();
  String tempStr = String(temperatura);
  String wilgStr = String(wilgotnosc);
  char charBuf[5];

    if (mqtt.connected()) {
      Serial.println("Publishing...");
      //switch(i){
        //case 0: {
          tempStr.toCharArray(charBuf,4);
          mqtt.publish("1262", charBuf); 
          Serial.println(charBuf);
        //  i++;
        //  break;  
       // }
       // case 1: {
         delay(3000);
          wilgStr.toCharArray(charBuf,4);
          mqtt.publish("1263", charBuf);
         Serial.println(charBuf);
       //   i=0; 
       //   break;
 //        }
    //  }
      Serial.println("Published.");
      }
  delay(1000);
 
}




/////////////////////////////////////////////////////////////////////

void DHT11Pomiar(){
  int chk = DHT11.read(DHT11PIN);
  Serial.println("\n");
  Serial.print("Read sensor: ");
  switch (chk)
  {
    case DHTLIB_OK: 
		//Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//Serial.println("Time out error"); 
		break;
    default: 
		//Serial.println("Unknown error"); 
		break;
  }
  if(chk==DHTLIB_OK)
  {
    wilgotnosc = (int)DHT11.humidity;
    temperatura = (int)DHT11.temperature;
    
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, 2);
  
    Serial.print("Temperature (Â°C): ");
    Serial.println((float)DHT11.temperature, 2);
  }
}

