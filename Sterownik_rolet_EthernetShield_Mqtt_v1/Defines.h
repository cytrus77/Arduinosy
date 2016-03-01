#ifndef DEFINES_H
#define DEFINES_H

#define ROLLER_COUNT       5
#define MAX_ROLLER_TIMEOUT 30  // in secunds
#define INT_TIMER_PERIOD   100000 // in us
#define ROLLER_TIMEOUT     MAX_ROLLER_TIMEOUT * 1000000 / INT_TIMER_PERIOD
#define UP                 true
#define DOWN               false
#define ON                 false
#define OFF                true

//Publish
#define MQTT_UPTIME        9000

//Subscribe
#define MQTT_ROLETA1       9500
#define MQTT_ROLETA2       9501
#define MQTT_ROLETA3       9502
#define MQTT_ROLETA4       9503
#define MQTT_ROLETA5       9504
#define MQTT_ALL_ROLETS    9510

//Pinout
#define ROLETA1UPPIN       2
#define ROLETA1DOWNPIN     3
//#define SSforSDCard        4 - reserved
#define ROLETA2UPPIN       5
#define ROLETA2DOWNPIN     6
#define ROLETA3UPPIN       7
#define ROLETA3DOWNPIN     8
#define ROLETA4UPPIN       9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define ROLETA4DOWNPIN     A0
#define ROLETA5UPPIN       A1
#define ROLETA5DOWNPIN     A2
#define STATUSLEDPIN       A3

#endif


