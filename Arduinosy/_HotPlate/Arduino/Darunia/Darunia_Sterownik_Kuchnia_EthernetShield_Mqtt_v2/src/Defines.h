#ifndef DEFINES_H
#define DEFINES_H

#define INT_TIMER_PERIOD   3000 // in us
#define MAX_ROLLER_TIMEOUT 30  // in secunds
#define ROLLER_TIMEOUT     MAX_ROLLER_TIMEOUT * 1000000 / INT_TIMER_PERIOD
#define ROLLER_IMPULS      500 * 1000 / INT_TIMER_PERIOD
#define SENSOR_SEND_PERIOD 10  // in seconds
#define SENS_SEND_CYCLE_PERIOD SENSOR_SEND_PERIOD * 1000000 / INT_TIMER_PERIOD
#define ROLLER_COUNT       3
////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        9000
#define MQTT_SUBSCRIBE     9050
#define MQTT_PIRSENSOR     9401
#define MQTT_LIGHTSENSOR   9701
#define MQTT_GASSENSOR     9901
#define MQTT_FLOODSENSOR   9902

//Subscribe
#define MQTT_DIMMER        9201
#define MQTT_ROLETA2       9501   // KUCHNIA FRONT
#define MQTT_ROLETA3       9502   // KUCHNIA BOK
#define MQTT_ROLETA1       9503   // SALON
#define MQTT_ALL_ROLETS    9500   // WSZYSTKIE ROLETY

//Pinout
#define ROLETA1UPPIN       2      // SALON
#define ROLETA1DOWNPIN     3      // SALON
//#define SSforSDCard        4 - reserved
#define DIMMERPIN          5
#define ROLETA2UPPIN       6
#define ROLETA2DOWNPIN     7
//#define NOTUSEDPIN         8
#define PIRSENSORPIN       9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define ROLETA3UPPIN       A0
#define ROLETA3DOWNPIN     A1
#define FLOODSENSORPIN     A2
#define LIGHTSENSORPIN     A3
#define GASSENSORPIN       A4
#define STATUSLEDPIN       A5
/////////////////// KUCHNIA  KONIEC ///////////////////////////////////////////////

#endif // end of #ifndef DEFINES_H
