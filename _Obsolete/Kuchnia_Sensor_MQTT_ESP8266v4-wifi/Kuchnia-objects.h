#ifndef KUCHNIA_OBJECTS_H
#define KUCHNIA_OBJECTS_H

#include <SoftwareSerial.h>
#include "Kuchnia-defines.h"

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";

int mqttPeriod = MQTTPERIOD*1000;
int i2cPeriod  = I2CPERIOD*1000;

int sendBuffer[16][2] = {};
char sendIndex = 0;
char reciIndex = 0;
int msCounterMqtt  = 0;
int msCounterI2C   = 0;
int mqttMode = MQTTCONFIG;

// Update these with values suitable for your network.
char server[] = "192.168.0.142";

#endif
