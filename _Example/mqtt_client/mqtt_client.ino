#include <NanodeUNIO.h>
#include <NanodeUIP.h>
#include <NanodeMQTT.h>

struct SensorNode
   {
  int                   nodeID;
  int			sensorID;		
//  unsigned long         var1_usl;
//  float                 var2_float;
//  float			var3_float;		//
//  int                   var4_int;
   } ;
   

//void sendMqttMsg(SensorNode* sensorNode);
NanodeMQTT mqtt(&uip);
struct timer my_timer;
struct SensorNode sensorNode1;

sensorNode1.nodeID = 15;
sensorNode1.sensorID = 1;
//sensorNode1.var1_usl = 111;
//sensorNode1.var2_float = 22.2;
//sensorNode1.var3_float = 33.3;
//sensorNode1.var4_int = 44;



void setup() {
  byte macaddr[6];
  NanodeUNIO unio(NANODE_MAC_DEVICE);

//  Serial.begin(38400);
//  Serial.println("MQTT Publish test");
  
  unio.read(macaddr, NANODE_MAC_ADDRESS, 6);
  uip.init(macaddr);

  // FIXME: use DHCP instead
  uip.set_ip_addr(192, 168, 137, 100);
  uip.set_netmask(255, 255, 255, 0);

  uip.wait_for_link();
//  Serial.println("Link is up");

  // Setup a timer - publish every 5 seconds
  timer_set(&my_timer, CLOCK_SECOND * 5);

  // FIXME: resolve using DNS instead
  mqtt.set_server_addr(192, 168, 137, 2);
  mqtt.connect();

//  Serial.println("setup() done");
}

void loop() {
  uip.poll();
  static char zmienna = '0';
  zmienna++;
  if(timer_expired(&my_timer)) {
    timer_reset(&my_timer);
    if (mqtt.connected()) {
  //    Serial.println("Publishing...");
      mqtt.publish("test", "Hello world");
      mqtt.publish("1262", "18.7"); 
  //    Serial.println("Published.");
      
    }
  }
}

/*

void sendMqttMsg(SensorNode* sensorNode)
{
      varnum = 3;
      char buff_topic[6];
      char buff_message[12];
      sprintf(buff_topic, "%02d%01d%01d", sensorNode.nodeID, sensorNode.sensorID, varnum);
     // Serial.println(buff_topic);
      dtostrf (sensorNode.var1_usl, 10, 1, buff_message);
      mqtt.publish(buff_topic, buff_message);

}*/
