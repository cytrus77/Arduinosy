#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include "Utils.h"

#include "Defines.h"
#include "Uptime.h"
#include "Dimmer.h"
#include "StatusLed.h"
#include "MqttSensor.h"
#include "Relay.h"
#include <DallasTemperature.h>


#define DEBUG 1

void callback(char* topic, byte* payload, unsigned int length);

byte mac[]    = { 0x10, 0x0B, 0xBB, 0x01, 0x1A, 0xE6 };
// byte ip[]     = { 192, 168, 100, 51 };
// byte server[] = { 192, 168, 100, 50 };
byte ip[]     = { 192, 168, 21, 253 };
byte server[] = { 192, 168, 21, 254 };

long lastMqttReconnectAttempt = 0;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);
Dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER, MQTT_DIMMET_TIMEOUT, DIMMER_TIMEOUT, CYCLES_PER_SECOND);

OneWire oneWire(DS18B20PIN);
DallasTemperature ds18b20(&oneWire);
unsigned int ds18b20timer = 0;

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  #ifdef DEBUG
  Serial.begin(19200);
  Serial.println("setup()");
  #endif

  Ethernet.begin(mac, ip);
  delay(1000);
  mqttConnect();

  StatusLed.setMode(statusled::poweron);
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);

  //Smooth dimming interrupt init
  Timer1.initialize(INT_TIMER_PERIOD);
  Timer1.attachInterrupt(TimerLoop);

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

    if (ds18b20timer > (CYCLES_PER_SECOND * 60))
    {
      measureTempAndSend();
      ds18b20timer = 0;
    }
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
  ledDimmer.processTimer();
  StatusLed.processTimer();
  ledDimmer.setDimmer();

  if (ledDimmer.getCurrentValue() == 0)
  {
    digitalWrite(RELAYPIN, HIGH);
  }

  ++ds18b20timer;
}

void measureTempAndSend()
{
  ds18b20.requestTemperatures();
  float temp = ds18b20.getTempCByIndex(0);
  sendMqtt(MQTT_TEMP, temp);
}

void sendMqtt(const String& topic, float value)
{
  char dataChar[8];
  ftoa(value, dataChar);
  client.publish(topic.c_str(), dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT float");
  Serial.println(topic.c_str());
  Serial.println(dataChar);
  #endif
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
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
  int data_int  = atoi((char *) p);

  if (topicStr == ledDimmer.getMqttTopic())
  {
    ledDimmer.setValue(data_int);

    if (data_int == 0)
    {
      #ifdef DEBUG
      Serial.println("RELAY - HIGH");
      #endif
    }
    else
    {
      digitalWrite(RELAYPIN, LOW);

      #ifdef DEBUG
      Serial.println("RELAY - LOW");
      #endif
    }
  }
  else if (topicStr == ledDimmer.getTimeoutMqttTopic())
  {
    ledDimmer.setTimeout(data_int*60);
  }

  // Free the memory
  free(p);
}

boolean mqttConnect()
{
  if (client.connect("Oswzew", "admin", "Isb_C4OGD4c3"))
  {
    Serial.println("Connected");
    delay(2000);
    sendAllSubscribers();
  }

  return client.connected();
}

void sendAllSubscribers(void)
{
    client.subscribe(ledDimmer.getMqttTopic().c_str());
    client.subscribe(ledDimmer.getTimeoutMqttTopic().c_str());
}
