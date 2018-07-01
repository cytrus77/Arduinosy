#pragma once
#include <string.h>

const unsigned int INT_TIMER_PERIOD = 10000; // in us
const unsigned int CYCLES_PER_SECOND = 1000000 / INT_TIMER_PERIOD;
const unsigned int DIMMER_TIMEOUT = 600;  // in seconds
////////////////////////////////////////////////////////////////////////////////
/////////////////// OSW ZEW ////////////////////////////////////////////////////

// MQTT topics
const String MQTT_IN   = "in";
const String MQTT_OUT  = "out";
const String MQTT_TEMP_BASE = MQTT_OUT + "/temp";
const String MQTT_DIMMER_BASE = MQTT_IN + "/dimmer";

// Publish
const String MQTT_UPTIME = MQTT_OUT + "/uptime";
const String MQTT_TEMP   = MQTT_TEMP_BASE + "/1";

// Subscribe
const String MQTT_DIMMER         = MQTT_DIMMER_BASE + "/1";
const String MQTT_DIMMET_TIMEOUT = MQTT_DIMMER_BASE + "/1timeout";

// // Publish
// const String MQTT_UPTIME = "u";
// const String MQTT_TEMP   = "t";
//
// // Subscribe
// const String MQTT_DIMMER         = "d";
// const String MQTT_DIMMET_TIMEOUT = "dt";


// Pinout
// const int RS485RO        = 0;
// const int RS485DI        = 1;
const int STATUSLEDPIN = 2;
const int DIMMERPIN    = 3;
// const int SSforSDCard    = 4;
const int DS18B20PIN   = 5;
const int RELAYPIN     = 6;
// const int NOTUSED  = 7;
// const int NOTUSED  = 8;
// const int NOTUSED  = 9;
// const int SSforEthernet  = 10;
// const int MOSIforEthernet = 11;
// const int MISOforEthernet = 12;
// const int SCKforEthernet = 13;
// const int NOTUSED   = A0;
// const int NOTUSED   = A1;
// const int NOTUSED  = A5;
// const int NOTUSED  = A4;
// const int NOTUSED  = A3;
// const int NOTUSED  = A2;

/////////////////// OSW ZEW KONIEC ///////////////////////////////////////////////
