#ifndef MQTTCONFIG_H
#define MQTTCONFIG_H

//#define DEBUG 1

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
  ledDimmer.registerSubscrib();
}

void mqttDisconnected(void* response)
{

}

void mqttData(void* response)
{
  int data = 0;
  int topic = 0;
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

  switch(topic)
  {
    case MQTT_DIMMER:
    {
      ledDimmer.setValue(data);
      break;
    }
    case MQTT_PHOTO_TRIGGER:
    {
      ledDimmer.setTrigger(data);
      break;
    }
    case MQTT_DIMMER_TIMER:
    {
      ledDimmer.setTimer(data);
      break;
    }
    default: 
    break;
  }
}

void mqttPublished(void* response)
{

}

#endif
