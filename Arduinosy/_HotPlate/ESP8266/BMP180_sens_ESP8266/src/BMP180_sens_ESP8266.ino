#include <ESP8266WiFi.h>   // ESP8266 wifi lib
#include <PubSubClient.h>  // MQTT
#include <string.h>
#include "Uptime.h"
#include "sensors.hpp"
#include "Utils.h"


// wifi config
const char* wifi_ssid = "cytrynowa";
const char* wifi_password = "limonkowy";
WiFiClient espClient;

// mqtt config
void callback(char* topic, byte* payload, unsigned int length);
byte mqtt_server[] = { 192, 168, 38, 11 };
const char* mqtt_name = "ESP_bmp180Sensor";
const char* mqtt_user = "admin";
const char* mqtt_password = "Isb_C4OGD4c3";
PubSubClient mqttClient(mqtt_server, 1883, callback, espClient);

const String devTopic = "/espBmp180/";
const String uptimeTopic = devTopic + "uptime";
const String smokeTopic = devTopic + "smoke";
const String bmpTempTopic = devTopic + "bmp180temp";
const String bmpAltitudeTopic = devTopic + "bmp180altitude";
const String bmpPressureTopic = devTopic + "bmp180pressure";


// uptime
uptime Uptime(uptimeTopic, &mqttClient);

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (mqttClient.connect(mqtt_name, mqtt_user, mqtt_password))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 1 seconds");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length)
{
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;

  #if DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  Serial.println();
  #endif

  // Free the memory
  free(p);
}

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  pinMode(sensors::smoke_pin, INPUT_PULLUP);
  sensors::bmp180.begin();

  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop()
{
  mqttClient.loop();

  if (!mqttClient.connected())
  {
    reconnect();
  }
  else
  {
    static long lastTime = millis();
    long now = millis();

    if (now - lastTime > 60000) // 60s
    {
      lastTime = now;
      Uptime.getUptime();
      Uptime.sendIfChanged();

      char dataChar[10];

      auto smoke = sensors::mq2Measure();
      itoa(smoke, dataChar, 10);
      mqttClient.publish(smokeTopic.c_str(), dataChar);
      #ifdef DEBUG
      Serial.print("smoke: ");
      Serial.println(smoke);
      #endif

      auto bmp180 = sensors::bmpMeasure();
      ftoa(bmp180.temp, dataChar);
      mqttClient.publish(bmpTempTopic.c_str(), dataChar);
      #ifdef DEBUG
      Serial.print("bmp180 temp: ");
      Serial.println(bmp180.temp);
      #endif

      ftoa(bmp180.altitude, dataChar);
      mqttClient.publish(bmpAltitudeTopic.c_str(), dataChar);
      #ifdef DEBUG
      Serial.print("bmp180 altitude: ");
      Serial.println(bmp180.altitude);
      #endif

      itoa(bmp180.pressure, dataChar, 10);
      mqttClient.publish(bmpPressureTopic.c_str(), dataChar);
      #ifdef DEBUG
      Serial.print("bmp180 pressure: ");
      Serial.println(bmp180.pressure);
      #endif
    }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////
