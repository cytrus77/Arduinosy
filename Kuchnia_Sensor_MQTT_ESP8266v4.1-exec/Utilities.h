#ifndef UTILITIES_H
#define UTILITIES_H

void sendUptime()
{
      static unsigned long uptime = 0;  
      int old_value = uptime;
      uptime = millis()/60000;
      if(old_value != uptime)
      {
         addToSendBuffer(MQTT_UPTIME, uptime);
      }
      #ifdef DEBUG
//      Serial.println("sendUptime()");
      #endif
}

void setMqttConfig()
{
  mqttMode = MQTTCONFIG;
  mqttSubscribCounter = 0;
  #ifdef DEBUG
  Serial.print("setMqttConfig() - mqttMode = ");
  Serial.println(mqttMode);
  #endif
}

void setMqttNormal()
{
  mqttMode = MQTTNORMAL;
  #ifdef DEBUG
  Serial.print("setMqttNormal() - mqttMode = ");
  Serial.println(mqttMode);
  #endif
}

void addToSendBuffer(int topic, int value)
{
   #ifdef DEBUG
   Serial.print("addToSendBuffer()() - m_mqttTopic = ");
   Serial.println(topic);
   if(sendBuffer[getIndex][0] != 0) Serial.println("addToSendBuffer() - buform przepelniony WRN!!!");
   Serial.print("addToSendBuffer() - getIndex = ");
   Serial.println(getIndex);
   #endif
   sendBuffer[getIndex][0] = topic;
   sendBuffer[getIndex][1] = value;
   getIndex = (getIndex+1) & 0x3F; 
}

int* get2Ints()
{
  int i = 0;
  static int data[2];
  byte x[2];
  byte y[2];
  while(Wire.available()) // loop through all but the last
  {
      if(i<2) x[i] = Wire.read(); 
      if(i>=2 && i<4) y[i-2] = Wire.read(); 
      i++; 
  }
  data[0] = (x[0] & 0xFF) | (((int)x[1] << 8) & 0xFF00);
  data[1] = (y[0] & 0xFF) | (((int)y[1] << 8) & 0xFF00);
  return data;
}

void send2Ints (int value1, int value2)
{
  byte data[4];
  data[0] = value1;
  data[1] = value1 >> 8;
  data[2] = value2;
  data[3] = value2 >> 8;
  Wire.write(data, 4);   
  #ifdef DEBUG
  Serial.println("send2Ints (int value1, int value2)");
  Serial.println(value1);
  Serial.println(value2);
  Serial.print("data1 = ");
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.print("data2 = ");
  Serial.println(data[2]);
  Serial.println(data[3]);
  #endif
}

#endif
