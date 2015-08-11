#ifndef KUCHNIA_H
#define KUCHNIA_H

#include <SoftwareSerial.h>
#include "Czujnik.h"
#include "Utilities.h"

#define MQTT_UPTIME        1000
#define MQTT_DIMMER        1020
#define MQTT_PHOTO_TRIGGER 1022
#define MQTT_DIMMER_TIMER  1021
#define MQTT_MOTION        1040
#define MQTT_PHOTO         1070
#define MQTT_GAS           1091
#define MQTT_FLOOD         1092

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";

sensor *sensor::sensorPtr[6];
int sensor::m_sensorCounter = 0;

//Sensor Vars
sensor gasSensor(MQTT_GAS, A5);
sensor floodSensor(MQTT_FLOOD, A3);
sensor photoSensor(MQTT_PHOTO, A4);
sensor motionSensor(MQTT_MOTION, 9);

// Update these with values suitable for your network.
char server[] = "192.168.0.142";

#endif
