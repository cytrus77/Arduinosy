#ifndef MQTTCONFIG_H
#define MQTTCONFIG_H

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
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
  debugPort.print("MQTT: Received: topic=");
  debugPort.println(topicStr);
  debugPort.print("data=");
  debugPort.println(dataStr);
  #endif
  
  topic = atoi(topicStr.c_str());
  data = atoi(dataStr.c_str());

  switch(topic) // do przerobienia
  {
 /*   case MQTT_RELAY:
    {
      mqttBuffer[MQTT_RELAY_NO].Data  = data;
      break;
    }
    case MQTT_DIMMER:
    {  
      mqttBuffer[MQTT_DIMMER_NO].Data = data;
      break;
    }
    default: */
    break;
  }
}

void mqttPublished(void* response)
{

}

#endif
