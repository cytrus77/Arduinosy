#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>
//#include <iostream.h>
#include <string.h>

using namespace std;

NanodeMQTT mqtt(&uip);
int zmienna = 0;

void message_callback(const char* topic, uint8_t* payload, int payload_length)
{
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, payload_length);
  uint8_t bufor[10];
  int i = 0;
  for(i = 0;i < payload_length; i++) {
    bufor[i] = *payload;
    payload ++;
  }
  bufor[i+1] = 0;
  zmienna = atoi((char *) bufor);
  //zmienna = *payload - 48;
  Serial.print(" Zmienna " );
  Serial.print(zmienna);
  Serial.println();
}



void setup() {
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

  Serial.begin(38400);
  Serial.println("MQTT Subscribe test");
  
  unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
  uip.init(macaddr);

  // FIXME: use DHCP instead
  uip.set_ip_addr(192, 168, 137, 101);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
  Serial.println("Link is up");

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 137, 2);
  mqtt.set_callback(message_callback);
  mqtt.connect();
  Serial.println("Connected to MQTT server");

  mqtt.subscribe("test");
  Serial.println("Subscribed.");

  Serial.println("setup() done");
  
  pinMode(2, OUTPUT);  
}

void loop() {
  uip.poll();
  digitalWrite(2, zmienna);
  
}
