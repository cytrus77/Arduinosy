#ifndef KUCHNIA_OBJECTS_H
#define KUCHNIA_OBJECTS_H

int loopCounter = 0;
int mqttMode = MQTTCONFIG;
int mqttSubscrib[5] = {MQTT_CONFIG_START, MQTT_DIMMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_TIMER, MQTT_CONFIG_END};
int mqttSubscribCounter = 0;
int sendBuffer[64][2] = {};
int sendIndex = 0;
int getIndex  = 0;
int measurePeriod = MEASUREPERIOD/2;
int pirLastValue = 0;

#endif

