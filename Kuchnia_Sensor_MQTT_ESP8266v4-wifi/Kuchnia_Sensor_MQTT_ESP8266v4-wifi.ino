#include <string.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>
#include "TimerOne.h"
#include "MqttConfig.h"
#include "Kuchnia-defines.h"
#include "Kuchnia-objects.h"
#include <avr/wdt.h>


/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);
#ifdef DEBUG
  debugPort.println("ARDUINO: startup....");
#endif
  Wire.begin();        // join i2c bus (address optional for master)
  
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

#ifdef DEBUG
  debugPort.println("ARDUINO: setup mqtt client");
#endif
  if(!mqtt.begin("DVES_duino", "admin", "Isb_C4OGD4c3", 120, 1)) {
    #ifdef DEBUG
    debugPort.println("ARDUINO: fail to setup mqtt");
    #endif
    while(1);
  }
  
  #ifdef DEBUG
  debugPort.println("ARDUINO: setup mqtt lwt");
  #endif
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  #ifdef DEBUG
  debugPort.println("ARDUINO: setup wifi");
  #endif
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(ssid,password);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(TimerLoop);
  #ifdef DEBUG
  debugPort.println("ARDUINO: system started");
  #endif
  //mqttMode = MQTTCONFIG;
  mqttMode = MQTTNORMAL;
 // send2Ints (MQTTCONFIG, MQTTCONFIG, I2C_EXEC_ADDR);
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  esp.process();
  wdt_reset();
  if(mqttMode == MQTTCONFIG)
  {
      if(msCounterI2C > i2cPeriod)
      { 
        #ifdef DEBUG
        debugPort.println("msCounterI2C > i2cPeriod - opcja1");
        #endif
        msCounterI2C = 0; 
        debugPort.println("dupa3");
        Wire.requestFrom(I2C_EXEC_ADDR, 2);    // request 2 bytes from slave device #2
        debugPort.println("dupa4");
        int* ptr;
        ptr = get2Ints();
        debugPort.println("dupa1");
        if(*ptr)
        {
           char topicChar[5];
           itoa(*ptr, topicChar, 10);
           mqtt.subscribe(topicChar);
           #ifdef DEBUG
            debugPort.print("mqtt.subscribe - topicChar = ");
            debugPort.println(topicChar);
           #endif
        }
        else mqttMode = MQTTNORMAL;
        debugPort.println("dupa2");
      }
  }
  else if(mqttMode == MQTTNORMAL)
  {
      if(msCounterI2C > i2cPeriod)
      { 
         #ifdef DEBUG
         debugPort.println("msCounterI2C > i2cPeriod - opcja2");
         #endif
         msCounterI2C = 0; 
         Wire.requestFrom(I2C_EXEC_ADDR, 4);    // request 6 bytes from slave device #2
         int* ptr;
         ptr = get2Ints();
         if(*ptr)
         {
            sendBuffer[reciIndex][0] = *ptr;
            sendBuffer[reciIndex][0] = *(ptr+1);
            reciIndex = (reciIndex+1) & 0xF;
         }
      }
  }
  
  if(msCounterMqtt > mqttPeriod && mqttMode == MQTTNORMAL)
  {
        #ifdef DEBUG
        debugPort.println("msCounterMqtt > mqttPeriod ");
        #endif
     msCounterMqtt = 0;
     if(sendBuffer[sendIndex][0])
     {
        char topicChar[5];
        char dataChar[5];
        itoa(sendBuffer[sendIndex][0], topicChar, 10);
      	itoa(sendBuffer[sendIndex][1], dataChar, 10);
      	mqtt.publish(topicChar, dataChar);
        sendBuffer[sendIndex][0] = 0;
        sendBuffer[sendIndex][1] = 0;
        sendIndex = (sendIndex+1) & 0xF;
        #ifdef DEBUG
        debugPort.print("mqtt.publish(topicChar, ) = ");
        debugPort.println(topicChar);
        debugPort.print("mqtt.publish(, dataChar) = ");
        debugPort.println(dataChar);
        #endif
     }
     
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  msCounterMqtt++;
  msCounterI2C++;
}

///////////WIRE stuff///////////////////////////////////////////////////////////
int* get2Ints()
{
  int i = 0;
  static int data[2] = {};
  byte x[2] = {};
  byte y[2] = {};
  while(Wire.available()) // loop through all but the last
  {
     #ifdef DEBUG
     debugPort.println("Wire.available() - wszedlem");
     #endif
      if(i<2) x[i] = Wire.read(); 
      if(i>=2 && i<4) y[i-2] = Wire.read(); 
      i++; 
  }
  data[0] = (x[0] & 0xFF) | (((int)x[1] << 8) & 0xFF00);
  data[1] = (y[0] & 0xFF) | (((int)y[1] << 8) & 0xFF00);
#ifdef DEBUG
  debugPort.print("get2Ints - dat[0] = ");
  debugPort.println(data[0]);
  debugPort.print("get2Ints - dat[1] = ");
  debugPort.println(data[1]);
#endif
  return data;
}

