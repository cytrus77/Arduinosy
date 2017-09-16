#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Defines.h"
#include "Uptime.h"
#include "StatusLed.h"
#include "Mux.h"


#define DEBUG 1

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

byte mac[]    = {  0x10, 0x0B, 0xA9, 0xDE, 0xFE, 0xC7 };  // Roller
byte ip[]     = { 192, 168, 0, 143 };                     // Roller
byte server[] = { 192, 168, 0, 142 };

long lastMqttReconnectAttempt = 0;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);
statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);
Mux mux1(MUX1EN, MUX1S0, MUX1S1, MUX1S2, MUX1S3, MUX1SIGNAL, Mux::Output);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup()
{
  #ifdef DEBUG
  Serial.begin(19200);
  Serial.println("setup()");
  #endif

  Ethernet.begin(mac, ip);
  delay(1000);
  mqttConnect();

  //Smooth dimming interrupt init
  Timer1.initialize(INT_TIMER_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  StatusLed.setMode(statusled::poweron);
  wdt_enable(WDTO_4S);
  lastMqttReconnectAttempt = 0;

  delay(200);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop()
{
  wdt_reset();

  mux1.setOutput(0, LOW);
  mux1.setOutput(1, LOW);


  if(client.connected())
  {
     client.loop();
     Uptime.getUptime();
     Uptime.sendIfChanged();
     StatusLed.setMode(statusled::online);
  }
  else
  {
     #ifdef DEBUG
     Serial.println("Disconnected");
     #endif

     StatusLed.setMode(statusled::offline);
     long now = millis();
     if (now - lastMqttReconnectAttempt > 3000)
     {
        lastMqttReconnectAttempt = now;
        // Attempt to reconnect
        if (mqttConnect())
        {
           lastMqttReconnectAttempt = 0;
        }
     }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  StatusLed.processTimer();
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
  Serial.println();
  #endif

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = (char) *p;
  int data_int  = atoi((char *) p);

  if (String(MQTT_ROLLER_ALL) == topic)
  {

  }
  else
  {

  }

  // Free the memory
  free(p);
}

void sendMqtt(int topic, int value)
{
  char topicChar[16];
  char dataChar[16];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  client.publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}

boolean mqttConnect()
{
  if (client.connect("Roller", "admin", "Isb_C4OGD4c3"))
  {
    #ifdef DEBUG
    Serial.println("Connected");
    #endif
    delay(2000);
    sendAllSubscribers();
  }

  return client.connected();
}

void sendAllSubscribers(void)
{
  client.subscribe(MQTT_ROLLER_ALL);
}
