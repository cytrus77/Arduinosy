#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <Wire.h>
#include <OneWire.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Interface.h"
#include "Kuchnia-defines.h"
#include "Czujnik.h"
#include "Dimmer.h"
#include "dht11mqtt.h"
#include "Uptime.h"

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0x33, 0xBA, 0xFE, 0x11, 0xEF };
byte server[] = { 192, 168, 137, 2 };
byte ip[]     = { 192, 168, 137, 10 };

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

//Sensor Vars
dimmer ledDimmer(MQTT_DIMMER, LEDDIMMERPIN, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_STATUS, MQTT_DIMMER_TIMER_STATUS, MQTT_PHOTO_TRIGGER_STATUS);
OneWire  ds18b20   (DS18B20PIN); 
dht11mqtt DHT11    (MQTT_TEMP,   MQTT_HUMIDEX,    DHT11PIN);
uptime Uptime      (MQTT_UPTIME);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  
  #ifdef DEBUG
  Serial.println("setup()");
  #endif
      
  Ethernet.begin(mac, ip);
  delay(5000);
  if (client.connect("DVES_duino", "admin", "Isb_C4OGD4c3")) 
  {
    Serial.println("Connected");
    delay(5000);
    client.publish("1000","9999");
    client.subscribe("inTopic");
  }
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  wdt_reset();
  client.loop();
  Serial.println("loop");

  if(client.connected())
  {
    Uptime.getUptime();        
    Serial.println("getTime - wysylanie");
    SendMqtt();
  //  delay(3000);
  }
  else
  {
     Serial.println("Rozlaczony");
     if (client.connect("DVES_duino", "admin", "Isb_C4OGD4c3")) 
     {
       Serial.println("Connected");
       delay(5000);
       client.publish("1000","9999");
       client.subscribe("inTopic");
     }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  static int pirLastValue = 0;
  if(pirLastValue)
  {
    if(ledDimmer.m_trigger)
    {
      if(ledDimmer.m_setValue > 25)
      {
        ledDimmer.resetTimer();
      }
      else
      {
        ledDimmer.setValue(100);
      }
    }
  }
  ledDimmer.setDimmer();
}

int* get2Ints()
{
	int i = 0;
	static int data[2];
	byte x[2];
	byte y[2];
	while(Wire.available()) // loop through all but the last
	{
		if(i<2) x[i] = Wire.read(); 
		if(i>=2 && i<4) y[i-2] = Wire.read(); 
		i++; 
	}
	data[0] = (x[0] & 0xFF) | (((int)x[1] << 8) & 0xFF00);
	data[1] = (y[0] & 0xFF) | (((int)y[1] << 8) & 0xFF00);
	return data;
}

void send2Ints (int value1, int value2)
{
	byte data[4];
	data[0] = value1;
	data[1] = value1 >> 8;
	data[2] = value2;
	data[3] = value2 >> 8;
	Wire.write(data, 4);
	
	#ifdef DEBUG
	Serial.println("send2Ints (int value1, int value2)");
	Serial.println(value1);
	Serial.println(value2);
	Serial.print("data1 = ");
	Serial.println(data[0]);
	Serial.println(data[1]);
	Serial.print("data2 = ");
	Serial.println(data[2]);
	Serial.println(data[3]);
	#endif
}

bool getDiffTime(unsigned long* timer, int diffInSec)
{
         if((millis() - *timer)/1000 > diffInSec)
         {
           	#ifdef DEBUG
	        Serial.println("getDiffTime() - return true; ");
                Serial.println(*timer);
                Serial.println((millis() - *timer)/1000);
                Serial.println(diffInSec);                
                #endif
               *timer = millis();
               return true; 
         }
         else return false;
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  
  #ifdef DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  #endif
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = atoi((char *) p);

  switch(atoi(topic))
  {
/*    case MQTT_RELAY:
    {
 //     mqttBuffer[MQTT_RELAY_NO].Data  = data;
      break;
    }
    case MQTT_DIMMER:
    {  
  //    mqttBuffer[MQTT_DIMMER_NO].Data = data;
      break;
    }*/
    default:
    break;
  }
  // Free the memory
  free(p);
}

void SendMqtt(){
  static int counter = 0;
  counter++;
  char dataChar[6];
  char topicChar[6];
  itoa(counter, dataChar, 10);
  itoa(MQTT_UPTIME, topicChar, 10);
  client.publish(topicChar, dataChar);
      
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}
