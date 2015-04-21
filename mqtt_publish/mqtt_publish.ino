#include <SPI.h>
#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>
#include <dht11.h>
#include <Ethernet.h>

#define DHT11PIN 2

dht11 DHT11;
NanodeMQTT mqtt(&uip);
struct timer my_timer;
signed int temperatura;
unsigned int wilgotnosc;
EthernetServer server = EthernetServer(1025);


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

  // Setup a timer - publish every 5 seconds
  timer_set(&my_timer, CLOCK_SECOND * 5);

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 137, 2);
  mqtt.connect();

 mqtt.subscribe("test");
  Serial.println("Subscribed.");
  server.begin();
  Serial.println("setup() done");
}




void loop() {
  uip.poll();
//  DHT11Pomiar();
//  String tempStr = String(temperatura);
//  String wilgStr = String(wilgotnosc);
//  char charBuf[20];

  if(1){//timer_expired(&my_timer)) {
    timer_reset(&my_timer);
    if (mqtt.connected()) {
      Serial.println("Publishing...");
      mqtt.publish("test", "Hello world");
  //    tempStr.toCharArray(charBuf,20);
    //  mqtt.publish("1262", charBuf); 
  //    wilgStr.toCharArray(charBuf,20);
  //    mqtt.publish("1263", charBuf); 
      Serial.println("Published.");
      
    }
  }
//  delay(1000);
  EthernetClient client = server.available();
  if (client == true) {
    // read bytes from the incoming client and write them back
    // to any clients connected to the server:
    Serial.println(client.read());
  }
  
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
  
    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, 2);
  }
}
