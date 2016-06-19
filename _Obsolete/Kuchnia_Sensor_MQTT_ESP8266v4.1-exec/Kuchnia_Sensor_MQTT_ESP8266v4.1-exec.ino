#include <string.h>
#include <Wire.h>
#include <avr/wdt.h>
#include "TimerOne.h"

#include "Interface.h"
#include "Kuchnia-defines.h"
#include "Kuchnia-objects.h"
#include "Czujnik.h"
#include "Dimmer.h"


//Sensor Vars
sensor gasSensor   (MQTT_GAS,   A1, NORMALSCALE,   ANALOGTYPE);
sensor floodSensor (MQTT_FLOOD, A3, INVERTEDSCALE, ANALOGTYPE);
sensor photoSensor (MQTT_PHOTO, A2, INVERTEDSCALE, ANALOGTYPE);
sensor motionSensor(MQTT_MOTION, 9, INVERTEDSCALE, DIGITALTYPE);
dimmer ledDimmer(MQTT_DIMMER, 6, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_STATUS, MQTT_DIMMER_TIMER_STATUS, MQTT_PHOTO_TRIGGER_STATUS);

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
   if(sendBuffer[sendIndex][0] >= 1000 && sendBuffer[sendIndex][0] <= 10000)
   {
      #ifdef DEBUG
      Serial.print("requestEvent() - sendIndex = ");
      Serial.println(sendIndex);
      #endif
      send2Ints(sendBuffer[sendIndex][0], sendBuffer[sendIndex][1]);
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

