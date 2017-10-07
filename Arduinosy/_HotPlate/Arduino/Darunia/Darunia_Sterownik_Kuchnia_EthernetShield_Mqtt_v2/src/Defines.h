#pragma once
#include <string.h>

#define INT_TIMER_PERIOD   3000 // in us
#define MAX_ROLLER_TIMEOUT 30  // in secunds
#define ROLLER_TIMEOUT     MAX_ROLLER_TIMEOUT * 1000000 / INT_TIMER_PERIOD
#define ROLLER_IMPULS      500 * 1000 / INT_TIMER_PERIOD
#define SENSOR_SEND_PERIOD 10  // in seconds
#define SENS_SEND_CYCLE_PERIOD SENSOR_SEND_PERIOD * 1000000 / INT_TIMER_PERIOD
#define ROLLER_COUNT       3
////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////

// MQTT topics
const String MQTT_HOME = "darunia";
const String MQTT_ROOM = "kuchnia";
const String MQTT_BASE = MQTT_HOME + "/" + MQTT_ROOM;
const String MQTT_IN   = MQTT_BASE + "/in";
const String MQTT_OUT  = MQTT_BASE + "/out";
const String MQTT_ROLLER_BASE = MQTT_IN + "/roller";
// Publish
const String MQTT_UPTIME      = MQTT_OUT + "/uptime";
const String MQTT_PIRSENSOR   = MQTT_OUT + "/pir";
const String MQTT_LIGHTSENSOR = MQTT_OUT + "/light";
const String MQTT_GASSENSOR   = MQTT_OUT + "/gas";
const String MQTT_FLOODSENSOR = MQTT_OUT + "/flood";
const String MQTT_DS18B20     = MQTT_OUT + "/temp/DS";
// Subscribe
const String MQTT_DIMMER         = MQTT_IN + "/dimmer/level";
const String MQTT_DIMMET_TIMEOUT = MQTT_IN + "/dimmer/timeout";
const String MQTT_DIMMER_TRIGGER = MQTT_IN + "/dimmer/trigger";
const String MQTT_DIMMER_PIRON   = MQTT_IN + "/dimmer/pir";
const String MQTT_ROLLER_SALON   = MQTT_ROLLER_BASE + "/salon";
const String MQTT_ROLLER_KUCHNIAF = MQTT_ROLLER_BASE + "/kuchniafront";
const String MQTT_ROLLER_KUCHNIAB = MQTT_ROLLER_BASE + "/kuchniabok";
const String MQTT_ROLLER_ALL     = MQTT_ROLLER_BASE + "/all";


// Pinout
// const int RS485RO        = 0;
// const int RS485DI        = 1;
const int RS485DE        = 2;
const int RS485RE        = 3;
// const int SSforSDCard    = 4;
const int PIRPIN         = 5;
const int DS18B20PIN     = 6;
// const int NOTUSED        = 7;
const int STATUSLEDPIN   = 8;
const int DIMMERPIN      = 9;
// const int SSforEthernet  = 10;
// const int MOSIforEthernet = 11;
// const int MISOforEthernet = 12;
// const int SCKforEthernet = 13;
const int LIGHTSENSORPIN = A0;
const int GASSENSORPIN   = A1;
const int FLOODSENSORPIN = A2;
// const int NOTUSED        = A3;
// const int NOTUSED        = A4;
// const int NOTUSED        = A5;

/////////////////// KUCHNIA  KONIEC ///////////////////////////////////////////////
