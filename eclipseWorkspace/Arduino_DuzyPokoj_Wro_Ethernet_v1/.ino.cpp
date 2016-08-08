//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2016-06-24 02:45:33

#include "Arduino.h"
#include "SPI.h"
#include <Ethernet.h>
#include "PubSubClient.h"
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include <OneWire.h>
#include <DallasTemperatureMqtt.h>
#include <DHTMqtt.h>
#include "Defines.h"
#include "Uptime.h"
#include "Dimmer.h"
#include "DimmerPir.h"
#include "StatusLed.h"
#include "MqttSensor.h"
#include "Utils.h"
void setup() ;
void loop() ;
void TimerLoop() ;
void callback(char* topic, byte* payload, unsigned int length) ;
void sendMqtt(int topic, int value) ;
void sendMqtt(int topic, float value) ;
boolean mqttConnect() ;


#include "Arduino_DuzyPokoj_Wro_Ethnernet_v1.ino"