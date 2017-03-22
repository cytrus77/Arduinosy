#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Utils.h"
#include "Defines.h"
#include "Uptime.h"
#include "Roller.h"
#include "Dimmer.h"
#include "StatusLed.h"
#include "MqttSensor.h"

#define DEBUG 1


void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

// Update these with values suitable for your network.
#ifdef KUCHNIA
byte mac[]    = {  0xBA, 0xCA, 0xF1, 0xFA, 0x11, 0x01 };  // Kuchnia
byte ip[]     = { 192, 168, 17, 31 };                     // Kuchnia
#endif
#ifdef PODDASZE
byte mac[]    = {  0xBA, 0xCA, 0xF1, 0xFA, 0x11, 0x02 };  // Poddasze
byte ip[]     = { 192, 168, 17, 32 };                     // Poddasze
#endif

byte server[] = { 192, 168, 17, 30 };
long lastMqttReconnectAttempt = 0;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME);

//Sensor Vars
roller Roller1(MQTT_ROLETA1, ROLETA1UPPIN, ROLETA1DOWNPIN, ROLLER_TIMEOUT);
roller Roller2(MQTT_ROLETA2, ROLETA2UPPIN, ROLETA2DOWNPIN, ROLLER_TIMEOUT);
roller Roller3(MQTT_ROLETA3, ROLETA3UPPIN, ROLETA3DOWNPIN, ROLLER_TIMEOUT);
#ifdef PODDASZE
roller Roller4(MQTT_ROLETA4, ROLETA4UPPIN, ROLETA4DOWNPIN, ROLLER_TIMEOUT);
roller Roller5(MQTT_ROLETA5, ROLETA5UPPIN, ROLETA5DOWNPIN, ROLLER_TIMEOUT);

roller* RollerTab[] = {&Roller1, &Roller2, &Roller3, &Roller4, &Roller5};
#endif

#ifdef KUCHNIA
mqttSensor PirSensor  (MQTT_PIRSENSOR,   &client, PIRSENSORPIN,   DIGITALTYPE, NORMALSCALE, SENSOR_PERIOD);
mqttSensor LightSensor(MQTT_LIGHTSENSOR, &client, LIGHTSENSORPIN, ANALOGTYPE, NORMALSCALE, SENSOR_PERIOD);
mqttSensor FloodSensor(MQTT_FLOODSENSOR, &client, FLOODSENSORPIN, ANALOGTYPE, NORMALSCALE, SENSOR_PERIOD);
mqttSensor GasSensor  (MQTT_GASSENSOR,   &client, GASSENSORPIN,   ANALOGTYPE, NORMALSCALE, SENSOR_PERIOD);
dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER);
roller* RollerTab[] = {&Roller1, &Roller2, &Roller3};
#endif
/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  #ifdef DEBUG
  Serial.begin(19200);
  Serial.println("setup()");
  #endif

  Ethernet.begin(mac, ip);
  delay(2000);
  mqttConnect();

  //Smooth dimming interrupt init
  Timer1.initialize(INT_TIMER_PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  StatusLed.setMode(statusled::poweron);
  wdt_enable(WDTO_4S);
  lastMqttReconnectAttempt = 0;
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
  #ifdef KUCHNIA
  ledDimmer.setDimmer();
  #endif
}

void allRollerOff()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);
    RollerTemp.stop();
  }
}

void allRollerUp()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);
    RollerTemp.up();
  }
}

void allRollerDown()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);
    RollerTemp.down();
  }
}

void allRollerSetState(bool state, bool direction)
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);
    RollerTemp.setState(state, direction);
  }
}


void allRollerCheckTimeout()
{
  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);
    RollerTemp.checkTimeout();
  }
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  int topic_int = 0;

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
  topic_int = atoi(topic);
  int data_int  = atoi((char *) p);

  for (int i = 0; i < ROLLER_COUNT; ++i)
  {
    roller& RollerTemp = *(RollerTab[i]);

    if (topic_int == RollerTemp.getTopic())
    {
      bool state = (data == 'U' || data == 'D') ? ROLLER_ON : ROLLER_OFF;
      bool direction = (data == 'U' ? ROLLER_UP : ROLLER_DOWN);
      RollerTemp.setState(state, direction);
    }
  }

  if (MQTT_ALL_ROLETS == topic_int)
  {
    bool state = (data == 'U' || data == 'D') ? ROLLER_ON : ROLLER_OFF;
    bool direction = (data == 'U' ? ROLLER_UP : ROLLER_DOWN);
    allRollerSetState(state, direction);
  }

  #ifdef KUCHNIA
  if (topic_int == ledDimmer.getMqttTopic())
  {
    ledDimmer.setValue(data_int);
  }
  #endif

  // Free the memory
  free(p);
}

void sendMqtt(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  client.publish(topicChar, dataChar);

  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}

void sendMqtt(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  client.publish(topicChar, dataChar);

  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}

boolean mqttConnect()
{
  #ifdef KUCHNIA
  if (client.connect("Kuchnia", "admin_kuchnia", "Isb_C4OGD4c1")) // Kuchnia
  #endif
  #ifdef PODDASZE
  if (client.connect("Poddasze", "admin_poddasze", "Isb_C4OGD4c2")) // Poddasze
  #endif
  {
    Serial.println("Connected");
    delay(2000);
    char topicChar[6];

    for (int i = 0; i < ROLLER_COUNT; ++i)
    {
      roller& RollerTemp = *(RollerTab[i]);
      char topicChar[6];
      itoa(RollerTemp.getTopic(), topicChar, 10);
      client.subscribe(topicChar);

    }
    itoa(MQTT_ALL_ROLETS, topicChar, 10);
    client.subscribe(topicChar);
    #ifdef KUCHNIA
    itoa(ledDimmer.getMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    #endif
  }

  return client.connected();
}
