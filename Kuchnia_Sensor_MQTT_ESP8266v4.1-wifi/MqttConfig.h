#ifndef MQTTCONFIG_H
#define MQTTCONFIG_H

#include "Kuchnia-defines.h"
#include "Kuchnia-objects.h"
#include <Wire.h>

void send2Ints (int value1, int value2, int dst)
{
  byte data1[2];
  byte data2[2];
#ifdef DEBUG
  debugPort.print("send2Ints - value1 = ");
  debugPort.println(value1);
  debugPort.print("send2Ints - value2 = ");
  debugPort.println(value2);
#endif
  data1[0] = value1;
  data1[1] = value1 >> 8;
  data2[0] = value2;
  data2[1] = value2 >> 8;
  Wire.beginTransmission(dst); // transmit to device #2
  Wire.write(data1[0]); 
  Wire.write(data1[1]);  
  Wire.write(data2[0]); 
  Wire.write(data2[1]);  
  Wire.endTransmission();
}

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      #ifdef DEBUG
      debugPort.println("WIFI CONNECTED");
      #endif
      mqtt.connect(server, 1883, false);
    } else {
      mqtt.disconnect();
    }
  }
}

void mqttConnected(void* response)
{
  #ifdef DEBUG
  debugPort.println("MQTT: Connected");
  #endif
    {
    char topicChar[5];
    itoa(MQTT_CONFIG_START, topicChar, 10);
    mqtt.subscribe(topicChar);
  }
  send2Ints (MQTT_CONFIG_START, 1, I2C_EXEC_ADDR);
}

void mqttDisconnected(void* response)
{

}

void mqttData(void* response)
{
  int topic;
  int data;
  RESPONSE res(response);
  String topicStr = res.popString();
  String dataStr = res.popString();

  #ifdef DEBUG
  debugPort.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  debugPort.print("MQTT: Received: topic=");
  debugPort.println(topicStr);
  debugPort.print("data=");
  debugPort.println(dataStr);
  #endif

  topic = atoi(topicStr.c_str());
  data = atoi(dataStr.c_str());
  send2Ints (topic, data, I2C_EXEC_ADDR);
  if(topic == MQTT_CONFIG_START) mqttMode = MQTTCONFIG;
}

void mqttPublished(void* response)
{

}

#endif
