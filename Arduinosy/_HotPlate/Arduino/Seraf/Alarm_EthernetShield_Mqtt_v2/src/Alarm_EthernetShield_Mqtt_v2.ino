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

#define DEBUG 1

void callback(char* topic, byte* payload, unsigned int length);

byte mac[]    = {  0x10, 0x0B, 0xBB, 0x01, 0x1A, 0xE6 };
// byte ip[]     = { 192, 168, 100, 51 };
// byte server[] = { 192, 168, 100, 50 };
byte ip[]     = { 192, 168, 166, 51 };
byte server[] = { 192, 168, 166, 110 };

long lastMqttReconnectAttempt = 0;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);

//Sensor Vars
MqttSensor Pir1stOffice  (MQTT_PIR1STOFFICE,   &client, PIR1STOFFICEPIN, DIGITALTYPE, NORMALSCALE, PIR_SEND_CYCLE_PERIOD);
MqttSensor Gas1stOffice  (MQTT_GAS1STOFFICE,   &client, GAS1STOFFICEPIN, ANALOGTYPE,  NORMALSCALE, GAS_SEND_CYCLE_PERIOD);
MqttSensor Pir2ndOffice  (MQTT_PIR2NDOFFICE,   &client, PIR2NDOFFICEPIN, DIGITALTYPE, NORMALSCALE, PIR_SEND_CYCLE_PERIOD);
MqttSensor Gas2ndOffice  (MQTT_GAS2NDOFFICE,   &client, GAS2NDOFFICEPIN, ANALOGTYPE,  NORMALSCALE, GAS_SEND_CYCLE_PERIOD);
MqttSensor Pir3rdOffice  (MQTT_PIR3RDOFFICE,   &client, PIR3RDOFFICEPIN, DIGITALTYPE, NORMALSCALE, PIR_SEND_CYCLE_PERIOD);
MqttSensor Gas3rdOffice  (MQTT_GAS3RDOFFICE,   &client, GAS3RDOFFICEPIN, ANALOGTYPE,  NORMALSCALE, GAS_SEND_CYCLE_PERIOD);
// MqttSensor Pir4thOffice  (MQTT_PIR4THOFFICE,   &client, PIR4THOFFICEPIN, DIGITALTYPE, NORMALSCALE,   PIR_SEND_CYCLE_PERIOD);
// MqttSensor Gas4thOffice  (MQTT_GAS4THOFFICE,   &client, GAS4THOFFICEPIN, ANALOGTYPE,  INVERTEDSCALE, GAS_SEND_CYCLE_PERIOD);

relay Siren1 (SIREN1PIN, MQTT_SIREN1, MQTT_SIREN1_TIMEOUT, SIREN_TIMEOUT_CYCLE_PERIOD, RelayActiveState);
relay Siren2 (SIREN2PIN, MQTT_SIREN2, MQTT_SIREN2_TIMEOUT, SIREN_TIMEOUT_CYCLE_PERIOD, RelayActiveState);

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
    Pir1stOffice.doMeasureAndSendDataIfItsTimeAndValueChanged();
    Pir2ndOffice.doMeasureAndSendDataIfItsTimeAndValueChanged();
    Pir3rdOffice.doMeasureAndSendDataIfItsTimeAndValueChanged();
    // Pir4thOffice.doMeasureAndSendDataIfItsTimeAndValueChanged();
    Gas1stOffice.doMeasureAndSendDataIfItsTime();
    Gas2ndOffice.doMeasureAndSendDataIfItsTime();
    Gas3rdOffice.doMeasureAndSendDataIfItsTime();
    // Gas4thOffice.doMeasureAndSendDataIfItsTime();
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
  Pir1stOffice.processTimer();
  Pir2ndOffice.processTimer();
  Pir3rdOffice.processTimer();
  // Pir4thOffice.processTimer();
  Gas1stOffice.processTimer();
  Gas2ndOffice.processTimer();
  Gas3rdOffice.processTimer();
  // Gas4thOffice.processTimer();
  Siren1.processTimer();
  Siren2.processTimer();
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

  if (topicStr == Siren1.getMqttTopic())
  {
    String data_str((char *) p);
    Siren1.setState((data_str == ON_CMD) ? relay::ERelayState::On
                                         : relay::ERelayState::Off);
  }
  else if (topicStr == Siren1.getTimeoutMqttTopic())
  {
    int data_int  = atoi((char *) p);
    Siren1.setTimeout(data_int);
  }
  else if (topicStr == Siren2.getMqttTopic())
  {
    String data_str((char *) p);
    Siren2.setState((data_str == ON_CMD) ? relay::ERelayState::On
                                         : relay::ERelayState::Off);
  }
  else if (topicStr == Siren2.getTimeoutMqttTopic())
  {
    int data_int  = atoi((char *) p);
    Siren2.setTimeout(data_int);
  }

  // Free the memory
  free(p);
}

boolean mqttConnect()
{
  if (client.connect("Biuro", "admin", "Isb_C4OGD4c3"))
  {
    Serial.println("Connected");
    delay(2000);
    sendAllSubscribers();
  }

  return client.connected();
}

void sendAllSubscribers(void)
{
    client.subscribe(Siren1.getMqttTopic().c_str());
    client.subscribe(Siren1.getTimeoutMqttTopic().c_str());
    client.subscribe(Siren2.getMqttTopic().c_str());
    client.subscribe(Siren2.getTimeoutMqttTopic().c_str());
}
