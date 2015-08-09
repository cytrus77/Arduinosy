#ifndef KUCHNIA_H
#define KUCHNIA_H

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
unsigned long uptime = 0;

//Sensor Vars
dht11 DHT11;
sensor gasSensor(1091, A5);
sensor floodSensor(1092, A3);
sensor photoSensor(1070, A4);
sensor motionSensor(1040, 9);

dimmer ledDimmer(1020, 6);

// Update these with values suitable for your network.
char server[] = "192.168.0.142";

#endif
