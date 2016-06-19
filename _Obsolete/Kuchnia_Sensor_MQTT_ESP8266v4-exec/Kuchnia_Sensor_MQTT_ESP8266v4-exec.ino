#include <dht11.h>
#include <string.h>
#include <Wire.h>
#include "TimerOne.h"
#include "Kuchnia-defines.h"
#include "Kuchnia-objects.h"
#include "Czujnik.h"
#include "Utilities.h"
#include <avr/wdt.h>


#define DEBUG 1
//#define LOG 1


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
 // mqttMode = MQTTCONFIG;
  mqttMode = MQTTNORMAL;
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
      Serial.println("loopCounter > measurePeriod");
      #endif
      loopCounter = 0;
  }

//  #ifdef DEBUG
//  debugPort.println("!!!!!!!!!!!!!!!!MAIN LOOP!!!!!!!!!!!!!!!!!!!");
//  #endif
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  loopCounter++;
 
  if(pirLastValue != motionSensor.m_value)
  {
    if(motionSensor.m_value && ledDimmer.m_trigger >= photoSensor.m_value)
    {
      ledDimmer.setValue(100);
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

void send2Ints (int value1, int value2, int dst)
{
  byte data1[2];
  byte data2[2];
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

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
 #ifdef DEBUG
 Serial.println("requestEvent()");
 #endif
 if(mqttMode == MQTTCONFIG)
 {
    send2Ints(mqttSubscrib[mqttSubscribCounter], 1, I2C_EXEC_ADDR);
    mqttSubscribCounter++;
    if(mqttSubscrib[mqttSubscribCounter] == 0) mqttMode = MQTTNORMAL;
 }
 else if(mqttMode == MQTTNORMAL)
 {
   if(sendBuffer[sendIndex][0])
   {
      send2Ints(sendBuffer[sendIndex][0], sendBuffer[sendIndex][1], I2C_EXEC_ADDR);
      sendIndex++;
   }
   else send2Ints(0, 0, I2C_EXEC_ADDR);
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
    case MQTT_CONFIG:
    {
      mqttMode = MQTTCONFIG;
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
    break;
  }
}

