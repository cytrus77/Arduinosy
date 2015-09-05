#ifndef KUCHNIA_OBJECTS_H
#define KUCHNIA_OBJECTS_H

#include "Czujnik.h"
#include "Utilities.h"

int loopCounter = 0;
int mqttPeriod = MQTTPERIOD*500;
int mqttMode = MQTTCONFIG;
int mqttSubscrib[4] = {MQTT_DIMMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_TIMER, 0};
int mqttSubscribCounter = 0;
int sendBuffer[16][2] = {};
char sendIndex = 0;
char getIndex  = 0;

int measurePeriod = MEASUREPERIOD/2;
int pirLastValue = 0;

//Sensor Vars
sensor gasSensor   (MQTT_GAS,   A1, NORMALSCALE);
sensor floodSensor (MQTT_FLOOD, A3, INVERTEDSCALE);
sensor photoSensor (MQTT_PHOTO, A2, INVERTEDSCALE);
sensor motionSensor(MQTT_MOTION, 9, INVERTEDSCALE);

dimmer ledDimmer(MQTT_DIMMER, 6, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER);

#endif
