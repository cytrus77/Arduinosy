#ifndef KUCHNIA_DEFINES_H
#define KUCHNIA_DEFINES_H

#define TRIGERVALUE        50       // default value for photo trigger [%]
#define TIMERVALUE         120      // default value of timer [sec]
#define MEASUREPERIOD      100      // in miliseconds

//Publish
#define MQTT_UPTIME        2000
#define MQTT_TEMP_DS18     2030
#define MQTT_TEMP_DHT      2031
#define MQTT_MOTION        2040
#define MQTT_MOTION2       2041
#define MQTT_PHOTO         2070
#define MQTT_HUMIDEX       2090
#define MQTT_GAS           2091
#define MQTT_FLOOD         2092
#define MQTT_DIMMER_STATUS        2080
#define MQTT_PHOTO_TRIGGER_STATUS 2082
#define MQTT_DIMMER_TIMER_STATUS  2081

//Subscribe
#define MQTT_DIMMER        2020
#define MQTT_PHOTO_TRIGGER 2022
#define MQTT_DIMMER_TIMER  2021

#define DEBUG 1
//#define LOG 1

#define MOTIONSENSORPIN    3
//#define SSforSDCard        4 - reserved
#define ONE_WIRE_BUS       5
#define LEDDIMMERPIN       6
#define DHT22PIN           7
#define MOTIONSENSOR2PIN   8
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define GASSENSORPIN       A1
#define PHOTOSENSORPIN     A2
#define FLOODSENSORPIN     A3
//I2C                      A4
//I2C                      A5


#endif


