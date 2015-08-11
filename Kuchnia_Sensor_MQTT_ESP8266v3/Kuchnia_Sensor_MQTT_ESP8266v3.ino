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
  debugPort.println("startuje setup");
  esp.enable();
  delay(500);
  debugPort.println("startuje 1");
  esp.reset();
  delay(500);
  debugPort.println("startuje 2");
  while(!esp.ready());
  debugPort.println("startuje 3");
  debugPort.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("DVES_duino", "admin", "Isb_C4OGD4c3", 120, 1)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while(1);
  }
  
  debugPort.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(ssid,password);

  debugPort.println("ARDUINO: system started");
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  #ifdef DEBUG
  debugPort.println("Timer 1 wlaczony!!!!!!");
  #endif
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  static int counter = 0;
  esp.process();
  counter = (counter+1)%sensor::m_sensorCounter;
  sensor::sensorPtr[counter]->getValue();
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  static int loopCounter = 0;
  loopCounter++;
  if(!(loopCounter%2500))
  {
      static int mqttCounter = 0;
      mqttCounter = (mqttCounter+1)%sensor::m_sensorCounter;
      sensor::sensorPtr[mqttCounter]->sendMqtt();
  }
  
  static int pirValue = 0;
  static dimmer ledDimmer(MQTT_DIMMER, 6, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER); // hack 4 to quick mqtt subscribe

  if(pirValue != motionSensor.m_value)
  {
    if(motionSensor.m_value)
    {
      ledDimmer.setValue(100);
      ledDimmer.resetTimer();
      pirValue = motionSensor.m_value;
    }
    else
    {
      pirValue = motionSensor.m_value;
    }
  }
  ledDimmer.setDimmer();
}


