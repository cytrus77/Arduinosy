#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>
#include <Ethernet.h>
#include <SPI.h>

NanodeMQTT mqtt(&uip);
EthernetServer server = EthernetServer(1883);
void message_callback(const char* topic, uint8_t* payload, int payload_length)
{
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, payload_length);
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
  uip.set_ip_addr(192, 168, 137, 100);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
  Serial.println("Link is up");

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 137, 2);
  mqtt.set_callback(message_callback);
  mqtt.connect();
  Serial.println("Connected to MQTT server");

  mqtt.subscribe("test/test1");
  Serial.println("Subscribed.");
 // server.begin();
  Serial.println("setup() done");
}

void loop() {
  uip.poll();
   // EthernetClient client = server.available();
  //if (client == true) {
    // read bytes from the incoming client and write them back
    // to any clients connected to the server:
   // Serial.println(client.read());
 // }
}
