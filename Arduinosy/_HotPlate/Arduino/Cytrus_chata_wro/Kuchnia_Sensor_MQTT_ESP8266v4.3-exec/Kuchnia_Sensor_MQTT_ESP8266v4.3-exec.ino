#include <string.h>
#include <Wire.h>
#include <OneWire.h>
#include <dht11.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Interface.h"
#include "Kuchnia-defines.h"
#include "Czujnik.h"
#include "Dimmer.h"

int loopCounter         = 0;
int mqttMode            = MQTTCONFIG;
int mqttSubscrib[5]     = {MQTT_CONFIG_START, MQTT_DIMMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_TIMER, MQTT_CONFIG_END};
int mqttSubscribCounter = 0;
int* sendBuffer[64][2]  = {};
int sendIndex           = 0;
int getIndex            = 0;
int measurePeriod       = MEASUREPERIOD/2;
int pirLastValue        = 0;

//Sensor Vars
OneWire  ds18b20(DS18B20PIN); 
sensor gasSensor   (MQTT_GAS,    GASSENSORPIN,    NORMALSCALE,   ANALOGTYPE);
sensor floodSensor (MQTT_FLOOD,  FLOODSENSORPIN,  INVERTEDSCALE, ANALOGTYPE);
sensor photoSensor (MQTT_PHOTO,  PHOTOSENSORPIN,  INVERTEDSCALE, ANALOGTYPE);
sensor motionSensor(MQTT_MOTION, MOTIONSENSORPIN, INVERTEDSCALE, DIGITALTYPE);
dimmer ledDimmer(MQTT_DIMMER, LEDDIMMERPIN, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_STATUS, MQTT_DIMMER_TIMER_STATUS, MQTT_PHOTO_TRIGGER_STATUS);
dht11 DHT11;

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
  static int counter = 0;
  if(loopCounter > measurePeriod)
  {
      counter = (counter+1)%sensor::m_sensorCounter;
      sensor::sensorPtr[counter]->doMeasure();
      sendUptime();        
      #ifdef DEBUG
 //     Serial.println("loopCounter > measurePeriod");
      #endif
      loopCounter = 0;
  }
  
  static unsigned long DHT111Timer;
  
  if(getDiffTime(&DHT111Timer, 2*SENSORSENDPERIOD))
  {
      Timer1.setPeriod(100000);
      unsigned int time_start = millis();
      int mqttTopic = MQTT_TEMP;
      int* DHT111Ptr = getDHT11temp();
      addToSendBuffer(&mqttTopic, DHT111Ptr);
      mqttTopic = MQTT_HUMIDEX;
      addToSendBuffer(&mqttTopic, DHT111Ptr+1);
      unsigned int time_stop = millis();
      Serial.print("Czas na wykonanie DHT111 = ");
      Serial.println(time_stop - time_start);
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
   if(*sendBuffer[sendIndex][0] >= 1000 && *sendBuffer[sendIndex][0] <= 10000)
   {
      #ifdef DEBUG
      Serial.print("requestEvent() - sendIndex = ");
      Serial.println(sendIndex);
      #endif
      send2Ints(*sendBuffer[sendIndex][0], *sendBuffer[sendIndex][1]);
      sendBuffer[sendIndex][0] = 0;
      sendIndex = (sendIndex+1) & 0x3F;
   }
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

void sendUptime()
{
	static int uptime = 0;  
        static int mqttTopic = MQTT_UPTIME;
	int old_value = uptime;

	uptime = millis()/60000;
	if(old_value != uptime)
	{
		addToSendBuffer(&mqttTopic, &uptime);
	}
	#ifdef DEBUG
//	Serial.println("sendUptime()");
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

void addToSendBuffer(int* topic, int* value)
{
	#ifdef DEBUG
	Serial.print("addToSendBuffer()() - m_mqttTopic = ");
	Serial.println(*topic);
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

int* getDHT11temp()
{
  static int DHTData[2] = {};      // [0] - temp,  [1] - humidity
  int chk = DHT11.read(DHT11PIN);
  
  switch (chk)
  {
    case DHTLIB_OK: 
                DHTData[1] = (int)DHT11.humidity;
                DHTData[0] = (int)DHT11.temperature;
                #ifdef DEBUG
		Serial.println("OK");
                #endif
		break;
    case DHTLIB_ERROR_CHECKSUM:
                #ifdef DEBUG 
		Serial.println("Checksum error"); 
                #endif
		break;
    case DHTLIB_ERROR_TIMEOUT: 
                #ifdef DEBUG
		Serial.println("Time out error"); 
                #endif
		break;
    default: 
                #ifdef DEBUG
		Serial.println("Unknown error"); 
                #endif
		break;
  }
  #ifdef DEBUG
  Serial.print("Humidity (%): ");
  Serial.println(DHTData[1]);
  Serial.print("Temperature (°C): ");
  Serial.println(DHTData[0]);
  #endif
  
  return DHTData;
}
