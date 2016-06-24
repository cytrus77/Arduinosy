#ifndef DEFINES_H
#define DEFINES_H

static const unsigned long INT_TIMER_PERIOD = 2000; // in us
static const unsigned long PIR_PERIOD       =   2 *  100000 / INT_TIMER_PERIOD;  // values in ms - 200ms;
static const unsigned long LIGHT_PERIOD     =  15 * 1000000 / INT_TIMER_PERIOD;
static const unsigned long GAS_PERIOD       =  30 * 1000000 / INT_TIMER_PERIOD;
static const unsigned long TEMP_PERIOD      =  60 * 1000000 / INT_TIMER_PERIOD;  // values in second - 60s
static const unsigned long DIMMER_TIMEOUT   = 300 * 1000000 / INT_TIMER_PERIOD;  // values in second - 300s

////////////////////////////////////////////////////////////////////////////////
/////////////////// DUZY POKOJ /////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        2000
#define MQTT_TEMP_DHT      2031
#define MQTT_TEMP_DS18     2030
#define MQTT_HUMIDEX       2090
#define MQTT_PIR1SENSOR    2040
#define MQTT_PIR2SENSOR    2041
#define MQTT_NOISE         2091
#define MQTT_LIGHTSENSOR   2070
#define MQTT_GASSENSOR     2092

//Subscribe
#define MQTT_DIMMER         2020
#define MQTT_DIMMER_TIMEOUT 2021
#define MQTT_DIMMER_TRIGGER 2022
#define MQTT_DIMMER_PIR     2023

//Pinout
//#define NOTUSEDPIN         2
//#define NOTUSEDPIN         3
//#define SSforSDCard        4 - reserved
#define DS18B20PIN           5
#define DIMMERPIN            6
#define DHT11PIN             7
#define PIR1SENSORPIN        A4
#define PIR2SENSORPIN        A5
//#define SSforEthernet      10 - reserved
//#define MOSIforEthernet    11 - reserved
//#define MISOforEthernet    12 - reserved
//#define SCKforEthernet     13 - reserved
#define GASSENSORPIN         A0
//#define NOTUSEDPIN         A1
#define LIGHTSENSORPIN       A2
#define NOISESENSORPIN       A3
//#define NOTUSEDPIN         A4
#define STATUSLEDPIN         8
/////////////////// DUZY POKOJ  KONIEC //////////////////////////////////////////

#endif /* DEFINES_H */


