#include "SPI.h"
#include <Ethernet.h>
#include "PubSubClient.h"
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include <OneWire.h>
#include <DallasTemperatureMqtt.h>
#include <DHTMqtt.h>

#include "Defines.h"
#include "Uptime.h"
#include "Dimmer.h"
#include "DimmerPir.h"
#include "StatusLed.h"
#include "MqttSensor.h"
#include "Utils.h"

#define DEBUG 1


void callback(char* topic, byte* payload, unsigned int length);

// Update these with values suitable for your network.
byte mac[]    = { 0xDE, 0xAD, 0xC1, 0xF0, 0xAA, 0xB2 };
byte ip[]     = { 192, 168, 0, 221 };
byte server[] = { 192, 168, 0, 142 };

long lastMqttReconnectAttempt = 0;
bool pirStatus = false;

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

statusled  StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime     Uptime(MQTT_UPTIME, &client);
mqttSensor Pir1Sensor (MQTT_PIR1SENSOR,  &client, PIR1SENSORPIN,  DIGITALTYPE, NORMALSCALE,   PIR_PERIOD);
mqttSensor Pir2Sensor (MQTT_PIR2SENSOR,  &client, PIR2SENSORPIN,  DIGITALTYPE, NORMALSCALE,   PIR_PERIOD);
mqttSensor LightSensor(MQTT_LIGHTSENSOR, &client, LIGHTSENSORPIN, ANALOGTYPE,  INVERTEDSCALE, LIGHT_PERIOD);
mqttSensor GasSensor  (MQTT_GASSENSOR,   &client, GASSENSORPIN,   ANALOGTYPE,  NORMALSCALE,   GAS_PERIOD);
dimmer     ledDimmer  (DIMMERPIN, MQTT_DIMMER, MQTT_DIMMER_TIMEOUT, DIMMER_TIMEOUT);
dimmerPir  ledDimmerPir(MQTT_DIMMER_PIR, MQTT_DIMMER_TRIGGER, &ledDimmer, &pirStatus);
OneWire oneWire(DS18B20PIN);
DallasTemperatureMqtt ds18b20Sensor(MQTT_TEMP_DS18, &client, &oneWire, TEMP_PERIOD);
dhtMqtt dht11Sensor(MQTT_TEMP_DHT, MQTT_HUMIDEX, &client, DHT11PIN, TEMP_PERIOD);

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
    Pir1Sensor.sendDataIfValueChanged();
    Pir2Sensor.sendDataIfValueChanged();
    LightSensor.sendDataIfValueChanged();
    GasSensor.doMeasureAndSendDataIfItsTimeAndValueChanged();
    ds18b20Sensor.doMeasureAndSendDataIfItsTimeAndValueChanged();
    dht11Sensor.doMeasureAndSendDataIfItsTimeAndValueChanged();
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

  LightSensor.doMeasureIfItsTime();
  Pir1Sensor.doMeasureIfItsTime();
  Pir2Sensor.doMeasureIfItsTime();
  pirStatus = Pir1Sensor.getValue() || Pir2Sensor.getValue();
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  ledDimmerPir.checkSensors();
  ledDimmer.processTimer();
  ledDimmer.setDimmer();
  StatusLed.processTimer();
  Pir1Sensor.processTimer();
  Pir2Sensor.processTimer();
  LightSensor.processTimer();
  GasSensor.processTimer();
  ds18b20Sensor.processTimer();
  dht11Sensor.processTimer();
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

  if (topic_int == ledDimmer.getMqttTopic())
  {
      ledDimmer.setValue(data_int);
  }
  else if (topic_int == ledDimmer.getTimeoutMqttTopic())
  {
	  ledDimmer.setTimeout(data_int * 1000000 / INT_TIMER_PERIOD * 60);
  }
  else if (topic_int == ledDimmerPir.getLightMqttTopic())
  {
	  ledDimmerPir.setLightTrigger(data_int);
  }
  else if (topic_int == ledDimmerPir.getPirMqttTopic())
  {
	  ledDimmerPir.setPirFlag(data_int);
  }

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
  if (client.connect("DuzyPokoj", "admin_duzypokoj", "Isb_C4OGD4c1")) // Duzy pokoj
  {
    Serial.println("Connected");
    delay(2000);
    char topicChar[6];
    
    itoa(ledDimmer.getMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmer.getTimeoutMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmerPir.getLightMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmerPir.getPirMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmer.getMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmer.getMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
  }

  return client.connected();
}


