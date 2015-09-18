#ifndef KUCHNIA_DEFINES_H
#define KUCHNIA_DEFINES_H

#define I2C_EXEC_ADDR      2
#define TRIGERVALUE        50       // default value for photo trigger [%]
#define TIMERVALUE         120      // default value of timer [sec]
#define MEASUREPERIOD      100      // in miliseconds
#define SENSORSENDPERIOD   30       // period for send sensor value [sec]

#define ANALOGTYPE         0
#define DIGITALTYPE        1
#define INVERTEDSCALE      1
#define NORMALSCALE        0

#define MQTTCONFIG         0
#define MQTTNORMAL         1

#define MQTT_HEALTH_CHECK  9997
#define MQTT_CONFIG_START  9998
#define MQTT_CONFIG_END    9999
//Publish
#define MQTT_UPTIME        1000
#define MQTT_TEMP          1030
#define MQTT_MOTION        1040
#define MQTT_PHOTO         1070
#define MQTT_HUMIDEX       1090
#define MQTT_GAS           1091
#define MQTT_FLOOD         1092
#define MQTT_DIMMER_STATUS        1080
#define MQTT_PHOTO_TRIGGER_STATUS 1082
#define MQTT_DIMMER_TIMER_STATUS  1081

#define MQTT_PUBLISH_COUNTER      10

//Subscribe
#define MQTT_DIMMER        1020
#define MQTT_PHOTO_TRIGGER 1022
#define MQTT_DIMMER_TIMER  1021

#define DEBUG 1
//#define LOG 1

//esp8266 RXPIN            0
//esp8266 TXPIN            1
//DebugPORT RXPIN          2
//DebugPORT TXPIN          3
//esp8266 CHPDPIN          4
#define LEDDIMMERPIN       6
#define DHT11PIN           7
#define MOTIONSENSORPIN    9
#define DS18B20PIN         10
#define GASSENSORPIN       A1
#define PHOTOSENSORPIN     A2
#define FLOODSENSORPIN     A3
//I2C                      A4
//I2C                      A5

#endif

