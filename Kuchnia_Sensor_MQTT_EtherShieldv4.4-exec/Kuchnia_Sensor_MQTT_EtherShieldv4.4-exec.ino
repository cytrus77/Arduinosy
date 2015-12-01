#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <Wire.h>
#include <OneWire.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Interface.h"
#include "Kuchnia-defines.h"
#include "Czujnik.h"
#include "Dimmer.h"
#include "dht11mqtt.h"
#include "Uptime.h"

//EthernetClient ethClient;
//PubSubClient client(server, 1883, callback, ethClient);

int loopCounter         = 0;
int mqttMode            = MQTTCONFIG;
int mqttSubscrib[]      = {MQTT_CONFIG_START, MQTT_HEALTH_CHECK, MQTT_DIMMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_TIMER, MQTT_CONFIG_END};
int mqttSubscribCounter = 0;
int* sendBuffer[MQTT_PUBLISH_COUNTER][3]  = {};
int measurePeriod       = MEASUREPERIOD/2;
int pirLastValue        = 0;
int healthCheckTimer    = 0;

//Sensor Vars
dimmer ledDimmer(MQTT_DIMMER, LEDDIMMERPIN, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_STATUS, MQTT_DIMMER_TIMER_STATUS, MQTT_PHOTO_TRIGGER_STATUS);
sensor motionSensor(MQTT_MOTION, MOTIONSENSORPIN, INVERTEDSCALE, DIGITALTYPE);
sensor gasSensor   (MQTT_GAS,    GASSENSORPIN,    NORMALSCALE,   ANALOGTYPE);
sensor floodSensor (MQTT_FLOOD,  FLOODSENSORPIN,  INVERTEDSCALE, ANALOGTYPE);
sensor photoSensor (MQTT_PHOTO,  PHOTOSENSORPIN,  INVERTEDSCALE, ANALOGTYPE);
OneWire  ds18b20   (DS18B20PIN); 
dht11mqtt DHT11    (MQTT_TEMP,   MQTT_HUMIDEX,    DHT11PIN);
uptime Uptime      (MQTT_UPTIME);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  
  #ifdef DEBUG
  Serial.println("setup()");
  #endif
      
  Wire.begin(I2C_EXEC_ADDR);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent);
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  setMqttConfig();
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  wdt_reset();
  health_check();
  
  static int counter = 0;
  if(loopCounter > measurePeriod)
  {
      counter = (counter+1)%sensor::m_sensorCounter;
      sensor::sensorPtr[counter]->doMeasure();
      Uptime.getUptime();        
      #ifdef DEBUG
      Serial.println("loopCounter > measurePeriod");
      #endif
      loopCounter = 0;
  }
  
  static unsigned long DHT111Timer;
  if(getDiffTime(&DHT111Timer, 2*SENSORSENDPERIOD))
  {
      Timer1.setPeriod(100000);
      DHT11.doMeasure();
      Timer1.setPeriod(2000);
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  loopCounter++;
 
  if(pirLastValue != motionSensor.m_value)
  {
    if(motionSensor.m_value && ledDimmer.m_trigger >= photoSensor.m_value)
    {
      if(ledDimmer.m_setValue > 25)
      {
        ledDimmer.resetTimer();
      }
      else
      {
        ledDimmer.setValue(100);
      }
      pirLastValue = motionSensor.m_value;
    }
    else if (!motionSensor.m_value)
    {
      pirLastValue = motionSensor.m_value;
    }
  }
  ledDimmer.setDimmer();
  
}

////////////////////////////////////////////////////////////////////////////////
///////////WIRE stuff///////////////////////////////////////////////////////////
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
 healthCheckTimer = millis();   // reset HC timer
 #ifdef DEBUG
 Serial.println("requestEvent()");
 #endif
 if(mqttMode == MQTTCONFIG)
 {
    send2Ints(mqttSubscrib[mqttSubscribCounter], 1);
    mqttSubscribCounter++;
    if(mqttSubscrib[mqttSubscribCounter-1] == MQTT_CONFIG_END) 
    {
       setMqttNormal();
    }
 }
 else if(mqttMode == MQTTNORMAL)
 {
    sendMqttData();
 }
}

void receiveEvent(int howMany)
{ 
  int* ptr;
  #ifdef DEBUG
  Serial.println("receiveEvent(int howMany)");
  #endif
  ptr = get2Ints();
  #ifdef DEBUG
  Serial.print("receiveEvent(int howMany) - ptr = ");
  Serial.println(*ptr);
  #endif
  switch(*ptr)
  {
    case MQTT_CONFIG_START:
    {
      setMqttConfig();
      mqttSubscribCounter = 1;
    }
    case MQTT_DIMMER:
    {
      ledDimmer.setValue(*(ptr+1));
      break;
    }
    case MQTT_PHOTO_TRIGGER:
    {
      ledDimmer.setTrigger(*(ptr+1));
      break;
    }
    case MQTT_DIMMER_TIMER:
    {
      ledDimmer.setTimer(*(ptr+1));
      break;
    }
    default: 
    {
        #ifdef DEBUG
        Serial.println("Przyszla wiadomosc o nieznanym ID");
        #endif
        break;
    }
  }
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

bool getDiffTime(unsigned long* timer, int diffInSec)
{
         if((millis() - *timer)/1000 > diffInSec)
         {
           	#ifdef DEBUG
	        Serial.println("getDiffTime() - return true; ");
                Serial.println(*timer);
                Serial.println((millis() - *timer)/1000);
                Serial.println(diffInSec);                
                #endif
               *timer = millis();
               return true; 
         }
         else return false;
}

void(* resetFunc) (void) = 0;     //declare reset function at address 0

void health_check()
{
   long timer = millis();
   if(timer - healthCheckTimer > 70000)
   {
       resetFunc();
   }
}

void registerToMqttTable(int* topic, int* value, int* flag)
{
    static int counter = 0;
    if(counter < MQTT_PUBLISH_COUNTER)
    {
       sendBuffer[counter][0] = topic;
       sendBuffer[counter][1] = value;
       sendBuffer[counter][2] = flag;    
       ++counter;
    }
    else
    {
       #ifdef DEBUG
       Serial.println("WARNING! sendBuffer[] is to small !!!");
       #endif
    }
}

void sendMqttData()
{
  int counter = 0;
  while(!(*sendBuffer[counter][2]) && counter < MQTT_PUBLISH_COUNTER) ++counter;
  
  if(*sendBuffer[counter][0] >= 1000 && *sendBuffer[counter][0] <= 10000 && *sendBuffer[counter][2] > 0)
  {
      #ifdef DEBUG
      Serial.print("sendMqttData() - counter = ");
      Serial.println(counter);
      #endif
      send2Ints(*sendBuffer[counter][0], *sendBuffer[counter][1]);
      *sendBuffer[counter][2] = 0;
  }
  else
  {
      #ifdef DEBUG
      Serial.println("Nothing to send");
      #endif
  }
}

