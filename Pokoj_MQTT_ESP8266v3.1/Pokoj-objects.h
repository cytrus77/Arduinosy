#ifndef KUCHNIA_OBJECTS_H
#define KUCHNIA_OBJECTS_H

#include <SoftwareSerial.h>
#include "Czujnik.h"
#include "Utilities.h"

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";

sensor *sensor::sensorPtr[6];
int sensor::m_sensorCounter = 0;
bool mqttBusy = 0;
int loopCounter = 0;
int mqttPeriod = MQTTPERIOD*500;
int measurePeriod = MEASUREPERIOD/2;
int pirLastValue = 0;

//Sensor Vars
sensor gasSensor   (MQTT_GAS,   A5, NORMALSCALE);
sensor floodSensor (MQTT_FLOOD, A3, INVERTEDSCALE);
sensor photoSensor (MQTT_PHOTO, A4, INVERTEDSCALE);
sensor motionSensor(MQTT_MOTION, 9, INVERTEDSCALE);

dimmer ledDimmer(MQTT_DIMMER, 6, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_FB, MQTT_DIMMER_TIMER_FB, MQTT_PHOTO_TRIGGER_FB);

// Update these with values suitable for your network.
char server[] = "192.168.0.142";

#endif
