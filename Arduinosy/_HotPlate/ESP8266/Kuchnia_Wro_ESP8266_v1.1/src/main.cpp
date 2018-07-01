#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DS18B20_mqtt.h"

#include "Defines.h"
#include "Dimmer.h"
#include "DimmerPir.h"
#include "StatusLed.h"
#include "Uptime.h"
#include "MqttSensor.h"
#include "pwm.h"
#include "Utils.h"


#define DEBUG 1

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

#define wifi_ssid "cytrynowa_wro"
#define wifi_password "limonkowy"

#define mqtt_login "Kuchnia_Wro_TestX"
#define mqtt_user "admin"
#define mqtt_password "Isb_C4OGD4c3"

byte server[] = { 192, 168, 0, 103 };
long lastMqttReconnectAttempt = 0;

WiFiClient espClient;
PubSubClient client(server, 1883, callback, espClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);

MqttSensor PirSensor  (MQTT_PIRSENSOR,   &client, PIRSENSORPIN,   DIGITALTYPE, NORMALSCALE,   PIR_SEND_CYCLE_PERIOD);
MqttSensor LightSensor(MQTT_LIGHTSENSOR, &client, LIGHTSENSORPIN, ANALOGTYPE,  INVERTEDSCALE, PIR_SEND_CYCLE_PERIOD);
MqttSensor FloodSensor(MQTT_FLOODSENSOR, &client, FLOODSENSORPIN, DIGITALTYPE, INVERTEDSCALE, SENS_SEND_CYCLE_PERIOD);
MqttSensor GasSensor  (MQTT_GASSENSOR,   &client, GASSENSORPIN,   DIGITALTYPE, NORMALSCALE,   SENS_SEND_CYCLE_PERIOD);

Dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER, MQTT_DIMMET_TIMEOUT, DIMMER_TIMEOUT, CYCLES_PER_SECOND);
DimmerPir ledDimmerPir(MQTT_DIMMER, MQTT_DIMMER_TRIGGER, &ledDimmer, &PirSensor, &LightSensor);

OneWire oneWire(DS18B20PIN);
DallasTemperature DS18B20(&oneWire);
ds18b20mqtt DS18B20MQTT(MQTT_DS18B20, &client, &DS18B20, SENS_SEND_CYCLE_PERIOD);

void setup_wifi(void);
boolean mqttConnect(void);
void TimerLoop(void);
void sendAllSubscribers(void);
void subscribe(const String& topic);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup()");
  #endif

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimerLoop);
  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
  interrupts();

  DS18B20.begin();
  StatusLed.setMode(statusled::poweron);

  setup_wifi();
  //Smooth dimming interrupt init
  lastMqttReconnectAttempt = 0;

  Serial.println("StartedX");

  delay(200);
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  #ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  if(client.connected())
  {
    client.loop();
    Uptime.getUptime();
    Uptime.sendIfChanged();

    PirSensor.doMeasureAndSendDataIfItsTime();
    LightSensor.doMeasureAndSendDataIfItsTime();
    FloodSensor.doMeasureAndSendDataIfItsTime();
    GasSensor.doMeasureAndSendDataIfItsTime();
    DS18B20MQTT.doMeasureAndSendDataIfItsTime();

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

  ledDimmerPir.checkSensors();
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  ledDimmer.setDimmer();
  ledDimmer.processTimer();
  StatusLed.processTimer();

  PirSensor.processTimer();
  LightSensor.processTimer();
  FloodSensor.processTimer();
  GasSensor.processTimer();
  DS18B20MQTT.processTimer();

  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  String topicStr(topic);

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

  if (topicStr == ledDimmer.getMqttTopic())
    ledDimmer.setValue(data_int);
  else if (topicStr == ledDimmer.getTimeoutMqttTopic())
    ledDimmer.setTimeout(data_int*60);
  else if (topicStr == ledDimmerPir.getLightMqttTopic())
    ledDimmerPir.setLightTrigger(data_int);
  else if (topicStr == ledDimmerPir.getPirMqttTopic())
    ledDimmerPir.setPirFlag(!data_int);
  // Free the memory
  free(p);
}

boolean mqttConnect()
{
  if (client.connect(mqtt_login, mqtt_user, mqtt_password)) // Kuchnia
  {
    Serial.println("Connected");
    delay(200);
    sendAllSubscribers();
  }
  return client.connected();
}

void sendAllSubscribers(void)
{
    subscribe(ledDimmer.getMqttTopic());
    subscribe(ledDimmer.getTimeoutMqttTopic());
    subscribe(ledDimmerPir.getLightMqttTopic());
}

void subscribe(const String& topic)
{
    client.subscribe(topic.c_str());
    Serial.print("Subscribed ");
    Serial.println(topic);
}
