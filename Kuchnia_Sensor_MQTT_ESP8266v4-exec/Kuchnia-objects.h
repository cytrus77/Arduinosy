#ifndef KUCHNIA_OBJECTS_H
#define KUCHNIA_OBJECTS_H

#include "Czujnik.h"
#include "Utilities.h"

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

dimmer ledDimmer(MQTT_DIMMER, 6, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER);

#endif
