#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>

#include "Defines.h"
#include "Dimmer.h"
#include "StatusLed.h"
#include "Uptime.h"
#include "pwm.h"
#include "Utils.h"
#include <DallasTemperature.h>


#define DEBUG 1

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

#define mqtt_login     "Zewnetrzne_osw"
#define mqtt_user      "admin"
#define mqtt_password  "Isb_C4OGD4c3"

//#define wifi_ssid "Darunia_i_Tobik"
//#define wifi_password "tobiasz1986"
//byte server[] = { 192, 168, 17, 30 };
#define wifi_ssid "www.jambonet.pl M"
#define wifi_password "makarymakary"
byte server[] = { 192, 168, 1, 30 };
// #define wifi_ssid "cytrynowa_wro"
// #define wifi_password "limonkowy"
// byte server[] = { 192, 168, 0, 142 };

long lastMqttReconnectAttempt = 0;

WiFiClient espClient;
PubSubClient client(server, 1883, callback, espClient);

statusled StatusLed(STATUSLEDPIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &client);
dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER, MQTT_DIMMET_TIMEOUT, DIMMER_TIMEOUT, CYCLES_PER_SECOND);

OneWire oneWire(DS18B20PIN);
DallasTemperature ds18b20(&oneWire);
int ds18b20timer = 0;

void setup_wifi(void);
boolean mqttConnect(void);
void TimerLoop(void);
void sendAllSubscribers(void);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup()
{
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup()");
  #endif

  StatusLed.setMode(statusled::poweron);
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, HIGH);
  setup_wifi();
  //Smooth dimming interrupt init
  lastMqttReconnectAttempt = 0;

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimerLoop);
  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
  interrupts();

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
void loop()
{
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
  ledDimmer.setDimmer();
  ledDimmer.processTimer();
  StatusLed.processTimer();

  if (ledDimmer.getCurrentValue() == 0)
  {
    digitalWrite(RELAYPIN, HIGH);
  }

  ++ds18b20timer;

  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
}

void measureTempAndSend()
{
  ds18b20.requestTemperatures();
  float temp = ds18b20.getTempCByIndex(0);
  sendMqtt(MQTT_TEMP, temp);
}

void sendMqtt(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  client.publish(topicChar, dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT float");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}

void sendMqtt(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
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
    char topicChar[6];
    itoa(ledDimmer.getMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
    itoa(ledDimmer.getTimeoutMqttTopic(), topicChar, 10);
    client.subscribe(topicChar);
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length)
{
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
  else if (topic_int == ledDimmer.getTimeoutMqttTopic())
  {
    ledDimmer.setTimeout(data_int*60);
  }
  else if (topic_int == MQTT_SUBSCRIBE)
  {
    sendAllSubscribers();
  }

  // Free the memory
  free(p);
}
