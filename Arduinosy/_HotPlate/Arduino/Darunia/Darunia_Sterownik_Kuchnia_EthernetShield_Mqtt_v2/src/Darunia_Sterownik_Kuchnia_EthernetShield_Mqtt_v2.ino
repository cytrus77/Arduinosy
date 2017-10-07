#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Utils.h"

#include "Defines.h"
#include "Uptime.h"
#include "RollerRs485.h"
#include "Dimmer.h"
#include "StatusLed.h"
#include "MqttSensor.h"

#define DEBUG 1

void callback(char* topic, byte* payload, unsigned int length);

byte mac[]    = {  0x10, 0x0B, 0xA9, 0xD3, 0x1A, 0xE6 };  // Kuchnia
byte ip[]     = { 192, 168, 17, 31 };                     // Kuchnia
byte server[] = { 192, 168, 17, 30 };

// byte ip[]     = { 192, 168, 0, 228 };                     // Kuchnia
// byte server[] = { 192, 168, 0, 142 };
long lastMqttReconnectAttempt = 0;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);

//Sensor Vars
RollerRs485 Roller1(MQTT_ROLLER_SALON, ROLLER_TIMEOUT);
RollerRs485 Roller2(MQTT_ROLLER_SALON, ROLLER_TIMEOUT);
RollerRs485 Roller3(MQTT_ROLLER_SALON, ROLLER_TIMEOUT);
RollerRs485* RollerTab[] = {&Roller1, &Roller2, &Roller3};

MqttSensor PirSensor  (MQTT_PIRSENSOR,   &client, PIRPIN,         DIGITALTYPE, NORMALSCALE,   SENS_SEND_CYCLE_PERIOD);
MqttSensor LightSensor(MQTT_LIGHTSENSOR, &client, LIGHTSENSORPIN, ANALOGTYPE,  INVERTEDSCALE, SENS_SEND_CYCLE_PERIOD);
MqttSensor FloodSensor(MQTT_FLOODSENSOR, &client, FLOODSENSORPIN, ANALOGTYPE,  INVERTEDSCALE, SENS_SEND_CYCLE_PERIOD);
MqttSensor GasSensor  (MQTT_GASSENSOR,   &client, GASSENSORPIN,   ANALOGTYPE,  INVERTEDSCALE, SENS_SEND_CYCLE_PERIOD);

Dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER);


/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
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
void loop() {
  wdt_reset();

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

     allRollerOff();
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  allRollerCheckTimeout();
  StatusLed.processTimer();
  ledDimmer.setDimmer();
}

void allRollerOff()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    RollerRs485& rollerTemp = *(RollerTab[i]);
    rollerTemp.stop();
  }
}

void allRollerUp()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    RollerRs485& rollerTemp = *(RollerTab[i]);
    rollerTemp.up();
  }
}

void allRollerDown()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    RollerRs485& rollerTemp = *(RollerTab[i]);
    rollerTemp.down();
  }
}

void allRollerCheckTimeout()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    RollerRs485& rollerTemp = *(RollerTab[i]);
    rollerTemp.tick();
  }
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  String topicStr = String(topic);

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

  if (MQTT_ROLLER_ALL == topicStr)
  {
    if (data == 'U')
      allRollerUp();
    else if (data == 'D')
      allRollerDown();
    else
      allRollerOff();
  }
  else if (topicStr == ledDimmer.getMqttTopic())
  {
    ledDimmer.setValue(data_int);
  }
  else
  {
    for (int i = 0; i < ROLLER_COUNT; ++i)
    {
      RollerRs485& rollerTemp = *(RollerTab[i]);

      if (topicStr == rollerTemp.getMqttTopic())
      {
        if (data == 'U')
          rollerTemp.up();
        else if (data == 'D')
          rollerTemp.down();
        else
          rollerTemp.stop();
      }
    }
  }

  // Free the memory
  free(p);
}

boolean mqttConnect()
{
  if (client.connect("Kuchnia", "admin", "Isb_C4OGD4c3")) // Kuchnia
     //(client.connect("Kuchnia", "admin_kuchnia", "Isb_C4OGD4c1")) // Kuchnia
  {
    Serial.println("Connected");
    delay(2000);
    sendAllSubscribers();
  }

  return client.connected();
}

void sendAllSubscribers(void)
{
    for (int i = 0; i < ROLLER_COUNT; ++i)
    {
      RollerRs485& rollerTemp = *(RollerTab[i]);
      client.subscribe(rollerTemp.getMqttTopic().c_str());
    }

    client.subscribe(MQTT_ROLLER_ALL.c_str());
    client.subscribe(ledDimmer.getMqttTopic().c_str());
}
