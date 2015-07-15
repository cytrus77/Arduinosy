#ifndef KUCHNIA_H
#define KUCHNIA_H

#define MQ2PIN           A5
#define PIRPIN           9
#define PHOTOPIN         A4
#define DHT11PIN         10
#define RELAYPIN         5
#define KEYPIN           6
#define MQTTDIODAPIN     7
#define GASDIODAPIN      8
#define FLOODPIN         A3

//Subscribed MQTT topics
#define MQTT_RELAY       1010
#define MQTT_RELAY_NO    7
#define MQTT_DIMMER      1020
#define MQTT_DIMMER_NO   8

//Published MQTT topics
#define MQTT_UPTIME     1000
#define MQTT_UPTIME_NO  0
#define MQTT_GAS        1091
#define MQTT_GAS_NO     1
#define MQTT_FLOOD      1092
#define MQTT_FLOOD_NO   2
#define MQTT_PHOTO      1070
#define MQTT_PHOTO_NO   3
#define MQTT_TEMP       1030
#define MQTT_TEMP_NO    4
#define MQTT_HUMI       1090
#define MQTT_HUMI_NO    5
#define MQTT_MOTION     1040
#define MQTT_MOTION_NO  6

#define NO_OF_MQTT_ELEMENTS 9

/*
#define MQTT_UPTIME_NO  0
#define MQTT_GAS_NO     1
#define MQTT_FLOOD_NO   2
#define MQTT_PHOTO_NO   3
#define MQTT_TEMP_NO    4
#define MQTT_HUMI_NO    5
#define MQTT_MOTION_NO  6

int MQTT_UPTIME = 1000;
int MQTT_GAS = 1091;
int MQTT_FLOOD = 1092;
int MQTT_PHOTO = 1070;
int MQTT_TEMP = 1030;
int MQTT_HUMI = 1090;
int MQTT_MOTION = 1040; */

#define LIGHT_TIME 120   //czas swiecenia sie swiatla w sekundach



typedef struct{
  int Data;
  int Topic;
} mqttBufferType;

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";

mqttBufferType mqttBuffer[NO_OF_MQTT_ELEMENTS];

//DHT11 Vars
dht11 DHT11;

int currentDimmer = 0;
boolean mqtt_status = false;

// Update these with values suitable for your network.
char server[] = "192.168.0.142";

#endif

