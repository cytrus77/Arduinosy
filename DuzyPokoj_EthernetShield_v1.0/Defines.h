#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG 1

//Publish
#define MQTT_TEMP_DHT      2031
#define MQTT_TEMP_DS18     2030
#define MQTT_HUMIDEX       2090
#define MQTT_MOTION        2040
#define MQTT_MOTION2       2041
#define MQTT_NOISE         2091
#define MQTT_LIGHT         2070
#define MQTT_GAS           2092
#define MQTT_UPTIME        2000
//Subscribe
#define MQTT_DIMMER         2020
#define MQTT_DIMMER_TIMEOUT 2021
#define MQTT_DIMMER_TRIGGER 2022
#define MQTT_DIMMER_PIR     2023

// PINOUT
// PIN 0 - free
// PIN 1 - free
#define DEBUGRXPIN           2  // Serial Rx for Debug
#define DEBUGTXPIN           3  // Serial Tx for Debug
//#define SSforSDCard        4 - reserved
#define DS18B20PIN           5
#define DIMMERPIN            6
#define DHT11PIN             7
#define MOTIONPIN            8
#define MOTION2PIN           9
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define GASPIN               A0
#define NOISEPIN             A1
#define LIGHTPIN             A2


// measurement period
#define TIMER0PERIOD         2000 // 2ms
#define COUNTIN1SEC          1000000/TIMER0PERIOD
#define TEMPPERIOD           60   // 1min
#define NOISEPERIOD          10   // 10s
#define LIGHTPERIOD          10   // 10s
#define GASPERIOD            10   // 10s
#define UPTIMEPERIOD         30   // 30s


#endif


