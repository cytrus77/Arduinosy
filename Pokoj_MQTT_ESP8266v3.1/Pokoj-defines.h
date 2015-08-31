#ifndef KUCHNIA_DEFINES_H
#define KUCHNIA_DEFINES_H


#define TRIGERVALUE        105     // default value for photo trigger [%]
#define TIMERVALUE         10      // default value of timer [sec]
#define MQTTPERIOD         5       // in seconds
#define MEASUREPERIOD      100       // in miliseconds

#define INVERTEDSCALE      1
#define NORMALSCALE        0
//Publish
#define MQTT_MOTION        2040
#define MQTT_PHOTO         2070
#define MQTT_GAS           2091
#define MQTT_FLOOD         2092
//Subscribe feedback
#define MQTT_DIMMER_FB        2080
#define MQTT_PHOTO_TRIGGER_FB 2082
#define MQTT_DIMMER_TIMER_FB  2081
//Subscribe
#define MQTT_DIMMER        2020
#define MQTT_PHOTO_TRIGGER 2022
#define MQTT_DIMMER_TIMER  2021
#define MQTT_UPTIME        "2000"   //"" are needed because it's string for direct use



#endif
