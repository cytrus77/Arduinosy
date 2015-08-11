#include <dht11.h>
#include <string.h>
#include "TimerOne.h"
#include "Kuchnia.h"
#include "Czujnik.h"
#include "Utilities.h"
#include "MqttConfig.h"
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>

//#define DEBUG 1
//#define LOG 1


/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);

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

  #ifdef DEBUG
  debugPort.println("ARDUINO: system started");
  #endif
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  static int counter = 0;
  esp.process();
  counter = (counter+1)%sensor::m_sensorCounter;
  sensor::sensorPtr[counter]->getValue();
  
  sendUptime();
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  static int loopCounter = 0;
  loopCounter++;
  if(!(loopCounter%2500) && !mqttBusy)
  {
      mqttBusy = 1;
      static int mqttCounter = 0;
      mqttCounter = (mqttCounter+1)%sensor::m_sensorCounter;
      sensor::sensorPtr[mqttCounter]->sendMqtt();
      mqttBusy = 0;
  }
  
  static int pirValue = 0;
  if(pirValue != motionSensor.m_value && ledDimmer.m_trigger > photoSensor.m_value)
  {
    if(motionSensor.m_value)
    {
      ledDimmer.setValue(100);
      pirValue = motionSensor.m_value;
    }
    else
    {
      pirValue = motionSensor.m_value;
    }
  }
  ledDimmer.setDimmer();
}


