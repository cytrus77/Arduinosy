#ifndef KUCHNIA_DEFINES_H
#define KUCHNIA_DEFINES_H

#define I2C_EXEC_ADDR      2
#define TRIGERVALUE        50       // default value for photo trigger [%]
#define TIMERVALUE         120      // default value of timer [sec]
#define MQTTPERIOD         5        // in seconds
#define MEASUREPERIOD      2000     // in miliseconds

#define MQTTCONFIG         0
#define MQTTNORMAL         1

#define INVERTEDSCALE      1
#define NORMALSCALE        0

//Publish
#define MQTT_CONFIG        0000

#define MQTT_UPTIME        1000
#define MQTT_MOTION        1040
#define MQTT_PHOTO         1070
#define MQTT_GAS           1091
#define MQTT_FLOOD         1092
//Subscribe
#define MQTT_DIMMER        1020
#define MQTT_PHOTO_TRIGGER 1022
#define MQTT_DIMMER_TIMER  1021



#endif
