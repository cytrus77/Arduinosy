#pragma once
#include <string.h>

#define INT_TIMER_PERIOD   10000 // in us
#define TIMER_PERIOD_NODEMCU INT_TIMER_PERIOD * 80
#define CYCLES_PER_SECOND 1000000 / INT_TIMER_PERIOD

#define SENSOR_SEND_PERIOD 10  // in seconds
#define SENS_SEND_CYCLE_PERIOD SENSOR_SEND_PERIOD * CYCLES_PER_SECOND

#define PIR_SEND_PERIOD 1   // in seconds
#define PIR_SEND_CYCLE_PERIOD PIR_SEND_PERIOD * CYCLES_PER_SECOND

#define DIMMER_TIMEOUT  300  // in seconds

////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////

// Pinout
const int LIGHTSENSORPIN = A0;
const int STATUSLEDPIN   = D0;
const int PIRSENSORPIN   = D1;
const int FLOODSENSORPIN = D2;
const int FANPIN         = D3;
// const int NOTUSED     = D4 // BUILTIN LED
const int GASSENSORPIN   = D5;
const int DHT11PIN       = D6;
const int DS18B20PIN     = D7;
const int DIMMERPIN      = D8;


// MQTT topics
const String MQTT_HOME = "wroclaw";
const String MQTT_ROOM = "kuchnia";
const String MQTT_BASE = MQTT_HOME + "/" + MQTT_ROOM;
const String MQTT_IN   = MQTT_BASE + "/in";
const String MQTT_OUT  = MQTT_BASE + "/out";
// Publish
const String MQTT_UPTIME      = MQTT_OUT + "/uptime";
const String MQTT_PIRSENSOR   = MQTT_OUT + "/pir";
const String MQTT_LIGHTSENSOR = MQTT_OUT + "/light";
const String MQTT_GASSENSOR   = MQTT_OUT + "/gas";
const String MQTT_FLOODSENSOR = MQTT_OUT + "/flood";
const String MQTT_DS18B20     = MQTT_OUT + "/temp/DS";
const String MQTT_DHT11       = MQTT_OUT + "/temp/DHT";
// Subscribe
const String MQTT_DIMMER         = MQTT_IN + "/dimmer/level";
const String MQTT_DIMMET_TIMEOUT = MQTT_IN + "/dimmer/timeout";
const String MQTT_DIMMER_TRIGGER = MQTT_IN + "/dimmer/trigger";


/////////////////// KUCHNIA  KONIEC ///////////////////////////////////////////////
