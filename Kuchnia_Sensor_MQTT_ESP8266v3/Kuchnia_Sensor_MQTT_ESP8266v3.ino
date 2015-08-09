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
  int counter;
  esp.process();
  counter = (sensor::m_sensorCounter+1)%sensor::m_sensorCounter;
  sensor::sensorPtr[counter]->getValue();

}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////


void TimerLoop()
{

}


void DHT11Pomiar(){
  switch (DHT11.read(1))  //wpisac nr pin-u
  {
    case DHTLIB_OK:              
                #ifdef DEBUG
                debugPort.println("\n");
                debugPort.print("DHT 11 Read sensor: ");
                #endif
                
                if((int)DHT11.humidity != 0 && (int)DHT11.temperature != 0)
                {
                  //  mqttBuffer[MQTT_HUMI_NO].Data = (int)DHT11.humidity;
                  //  mqttBuffer[MQTT_TEMP_NO].Data = (int)DHT11.temperature;
                }
                
                #ifdef DEBUG
                debugPort.print("Humidity (%): ");
                debugPort.println((float)DHT11.humidity, 2);
              
                debugPort.print("Temperature (Â°C): ");
                debugPort.println((float)DHT11.temperature, 2);
                #endif

		//debugPort.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//debugPort.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//debugPort.println("Time out error"); 
		break;
    default: 
		//debugPort.println("Unknown error"); 
		break;
  }
}

