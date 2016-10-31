#ifndef DEFINES_H
#define DEFINES_H

#define INT_TIMER_PERIOD   3000 // in us
#define MAX_ROLLER_TIMEOUT 30  // in secunds
#define ROLLER_TIMEOUT     MAX_ROLLER_TIMEOUT * 1000000 / INT_TIMER_PERIOD
#define SENSOR_SEND_PERIOD 10  // in seconds
#define SENS_SEND_CYCLE_PERIOD SENSOR_SEND_PERIOD * 1000000 / INT_TIMER_PERIOD
#define ROLLER_COUNT       2
////////////////////////////////////////////////////////////////////////////////
/////////////////// GARAZ ////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        9002
#define MQTT_SUBSCRIBE     9050
#define MQTT_PIRSENSOR     9402
#define MQTT_LIGHTSENSOR   9702
#define MQTT_GASSENSOR     9902

//Subscribe
#define MQTT_ROLETA1       9509   // KOTLOWNIA
#define MQTT_ROLETA2       9510   // GARAZ
#define MQTT_ALL_ROLETS    9500   // WSZYSTKIE ROLETY

//Pinout
#define ROLETA1UPPIN       2
#define ROLETA1DOWNPIN     3
//#define SSforSDCard        4 - reserved
//#define NOTUSED       5
#define ROLETA2UPPIN       6
#define ROLETA2DOWNPIN     7
#define SMOKESENSORPIN     8
#define PIRSENSORPIN       9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
//#define NOTUSED     A0
//#define NOTUSED     A1
//#define NOTUSED     A2
#define LIGHTSENSORPIN     A3
#define GASSENSORPIN       A4
#define STATUSLEDPIN       A5
/////////////////// GARAZ  KONIEC ///////////////////////////////////////////////

#endif // end of #ifndef DEFINES_H
