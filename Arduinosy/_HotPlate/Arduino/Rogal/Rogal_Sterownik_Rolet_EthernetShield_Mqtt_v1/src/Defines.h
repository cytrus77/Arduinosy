#pragma once

#define INT_TIMER_PERIOD   3000 // in us
////////////////////////////////////////////////////////////////////////////////

//Publish
#define MQTT_UPTIME        "general/uptime"

//Subscribe
#define MQTT_ROLLER1       "roller/room1"
#define MQTT_ROLLER2       "roller/room2"
#define MQTT_ROLLER3       "roller/room3"
#define MQTT_ROLLER4       "roller/room4"
#define MQTT_ROLLER5       "roller/room5"
#define MQTT_ROLLER6       "roller/room6"
#define MQTT_ROLLER7       "roller/room7"
#define MQTT_ROLLER8       "roller/room8"
#define MQTT_ROLLER9       "roller/room9"
#define MQTT_ROLLER_ALL    "roller/all"

//Pinout
#define RX                 0
#define TX                 1
#define STATUSLEDPIN       2
// #define ROLETA1DOWNPIN     3
// //#define SSforSDCard        4 - reserved
// #define DIMMERPIN          5
// #define ROLETA2UPPIN       6
// #define ROLETA2DOWNPIN     7
// //#define NOTUSEDPIN         8
// #define PIRSENSORPIN       9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define MUX1EN           A0
#define MUX1S0           A1
#define MUX1S1           A2
#define MUX1S2           A3
#define MUX1S3           A4
#define MUX1SIGNAL       A5
