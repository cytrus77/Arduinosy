#ifndef DEFINES_H
#define DEFINES_H

#define INT_TIMER_PERIOD   10000 // in us
#define TIMER_PERIOD_NODEMCU INT_TIMER_PERIOD * 80
#define CYCLES_PER_SECOND 1000000 / INT_TIMER_PERIOD

#define SENSOR_SEND_PERIOD 10  // in seconds
#define SENS_SEND_CYCLE_PERIOD SENSOR_SEND_PERIOD * CYCLES_PER_SECOND

#define PIR_SEND_PERIOD 1   // in seconds
#define PIR_SEND_CYCLE_PERIOD PIR_SEND_PERIOD * CYCLES_PER_SECOND

#define DIMMER_TIMEOUT  180  // in seconds

////////////////////////////////////////////////////////////////////////////////
/////////////////// KUCHNIA ////////////////////////////////////////////////////
//Publish
#define MQTT_UPTIME        91000
#define MQTT_SUBSCRIBE     9050
#define MQTT_PIRSENSOR     91040
#define MQTT_LIGHTSENSOR   91070
#define MQTT_GASSENSOR     91091
#define MQTT_FLOODSENSOR   91092
#define MQTT_DS18B20       91030

//Subscribe
#define MQTT_DIMMER         91020
#define MQTT_DIMMET_TIMEOUT 91021
#define MQTT_DIMMER_TRIGGER 91022
#define MQTT_DIMMER_ONOFF   91023

//Pinout
#define GASSENSORPIN       A0
#define STATUSLEDPIN       D0
#define FANPIN             D1
#define FLOODSENSORPIN     D2
#define PIRSENSORPIN       D3
// #define NOTUSED         D4 // BUILTIN LED
#define DIMMERPIN          D5
#define DS18B20PIN         D6
#define DHT11PIN           D7
#define LIGHTSENSORPIN     D8

/////////////////// KUCHNIA  KONIEC ///////////////////////////////////////////////

#endif // end of #ifndef DEFINES_H
