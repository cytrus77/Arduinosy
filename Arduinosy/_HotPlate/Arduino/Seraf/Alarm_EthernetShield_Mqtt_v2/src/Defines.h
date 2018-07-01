#pragma once
#include <string.h>

const unsigned int INT_TIMER_PERIOD = 3000; // in us
const unsigned int PIR_SEND_PERIOD  = 1;    // in seconds
const unsigned int PIR_SEND_CYCLE_PERIOD = PIR_SEND_PERIOD * 1000000 / INT_TIMER_PERIOD;    // in cycles
const unsigned int GAS_SEND_PERIOD  = 30;    // in seconds
const unsigned int GAS_SEND_CYCLE_PERIOD = GAS_SEND_PERIOD * 1000000 / INT_TIMER_PERIOD;    // in cycles
const unsigned int SIREN_TIMEOUT_PERIOD  = 180;    // in seconds
const unsigned int SIREN_TIMEOUT_CYCLE_PERIOD = SIREN_TIMEOUT_PERIOD * 1000000 / INT_TIMER_PERIOD;    // in cycles

const String ON_CMD  = "ON";
const String OFF_CMD = "OFF";
const bool RelayActiveState = false;
////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////

// MQTT topics
// const String MQTT_HOME = "seraf";
// const String MQTT_ROOM = "biuro";
// const String MQTT_BASE = MQTT_HOME + "/" + MQTT_ROOM;
// const String MQTT_IN   = MQTT_BASE + "/in";
// const String MQTT_OUT  = MQTT_BASE + "/out";
const String MQTT_IN   = "in";
const String MQTT_OUT  = "out";
const String MQTT_PIR_BASE = MQTT_OUT + "/pir";
const String MQTT_GAS_BASE = MQTT_OUT + "/gas";
const String MQTT_SIREN_BASE = MQTT_IN + "/siren";

// Publish
const String MQTT_UPTIME      = MQTT_OUT + "/uptime";
const String MQTT_PIR1STOFFICE = MQTT_PIR_BASE + "/1";
const String MQTT_PIR2NDOFFICE = MQTT_PIR_BASE + "/2";
const String MQTT_PIR3RDOFFICE = MQTT_PIR_BASE + "/3";
const String MQTT_PIR4THOFFICE = MQTT_PIR_BASE + "/4";
const String MQTT_GAS1STOFFICE = MQTT_GAS_BASE + "/1";
const String MQTT_GAS2NDOFFICE = MQTT_GAS_BASE + "/2";
const String MQTT_GAS3RDOFFICE = MQTT_GAS_BASE + "/3";
const String MQTT_GAS4THOFFICE = MQTT_GAS_BASE + "/4";
// Subscribe
const String MQTT_SIREN1         = MQTT_SIREN_BASE + "/1";
const String MQTT_SIREN1_TIMEOUT = MQTT_SIREN_BASE + "/1timeout";
const String MQTT_SIREN2         = MQTT_SIREN_BASE + "/2";
const String MQTT_SIREN2_TIMEOUT = MQTT_SIREN_BASE + "/2timeout";

// Pinout
// const int RS485RO        = 0;
// const int RS485DI        = 1;
const int STATUSLEDPIN    = 2;
const int PIR4THOFFICEPIN = 3;
// const int SSforSDCard    = 4;
const int PIR3RDOFFICEPIN = 5;
const int PIR2NDOFFICEPIN = 6;
const int PIR1STOFFICEPIN = 7;
const int SIREN1PIN       = 8;
const int SIREN2PIN       = 9;
// const int SSforEthernet  = 10;
// const int MOSIforEthernet = 11;
// const int MISOforEthernet = 12;
// const int SCKforEthernet = 13;
// const int NOTUSED   = A0;
// const int NOTUSED   = A1;
const int GAS1STOFFICEPIN = A5;
const int GAS2NDOFFICEPIN = A4;
const int GAS3RDOFFICEPIN = A3;
const int GAS4THOFFICEPIN = A2;

/////////////////// KUCHNIA  KONIEC ///////////////////////////////////////////////
