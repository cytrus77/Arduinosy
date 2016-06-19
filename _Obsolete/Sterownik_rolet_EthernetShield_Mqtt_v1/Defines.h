#ifndef DEFINES_H
#define DEFINES_H

//#define PODDASZE
#define KUCHNIA

#define MAX_ROLLER_TIMEOUT 30  // in secunds
#define INT_TIMER_PERIOD   100000 // in us
#define ROLLER_TIMEOUT     MAX_ROLLER_TIMEOUT * 1000000 / INT_TIMER_PERIOD


#ifdef KUCHNIA

#define ROLLER_COUNT       3
////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        9000
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
#define ROLETA1UPPIN       2
#define ROLETA1DOWNPIN     3
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
#endif



#ifdef PODDASZE

#define ROLLER_COUNT       5
/////////////////// PODDASZE ////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        9001
#define MQTT_PIRSENSOR     9

//Subscribe
#define MQTT_ROLETA1       9504   // SALON DRZWI
#define MQTT_ROLETA2       9505   // SYPIALNIA
#define MQTT_ROLETA3       9506   // LAZIENKA
#define MQTT_ROLETA4       9507   // POKOJ TYL
#define MQTT_ROLETA5       9508   // POKOJ FRONT
#define MQTT_ALL_ROLETS    9500   // WSZYSTKIE ROLETY

//Pinout
#define ROLETA1UPPIN       2
#define ROLETA1DOWNPIN     3
//#define SSforSDCard        4 - reserved
#define ROLETA2UPPIN       5
#define ROLETA2DOWNPIN     6
#define ROLETA3UPPIN       7
#define PIRSENSORPIN       8
//#define NOTUSEDPIN         9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define ROLETA3DOWNPIN     A0
#define ROLETA4UPPIN       A1
#define ROLETA4DOWNPIN     A2
#define ROLETA5UPPIN       A3
#define ROLETA5DOWNPIN     A4
#define STATUSLEDPIN       A5
/////////////////// PODDASZE  KONIEC ///////////////////////////////////////////////
#endif

#endif


