#ifndef DEFINES_H
#define DEFINES_H

//Publish
#define MQTT_TEMP_DHT      1030
#define MQTT_TEMP_DS18     1031
#define MQTT_HUMIDEX       1090
#define MQTT_MOTION        1040
#define MQTT_LIGHT         1070
#define MQTT_GAS           1091
#define MQTT_UPTIME        1000

//Subscribe
#define MQTT_DIMMER         1020
#define MQTT_DIMMER_TIMEOUT 1021
#define MQTT_DIMMER_TRIGGER 1022
#define MQTT_DIMMER_PIR     1023

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
// PIN 9 - free
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define GASPIN               A0
#define NOISEPIN             A1
#define LIGHTPIN             A2


// measurement period
#define TIMER0PERIOD         3000 // 3ms
#define COUNTIN1SEC          1000000/TIMER0PERIOD
#define TEMPPERIOD           60   // 1min
#define LIGHTPERIOD          10   // 10s
#define GASPERIOD            10   // 10s
#define UPTIMEPERIOD         30   // 30s


#endif
