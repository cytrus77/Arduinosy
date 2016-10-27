#include <SPI.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include <espduino.h>
#include <mqtt.h>

#include "Defines.h"
#include "Dimmer.h"
#include "Uptime.h"

void ftoa(float Value, char* Buffer);

const char* ssid = "Darunia_i_Tobik";
const char* password = "tobiasz1986";
char server[] = "192.168.17.30";

long lastMqttReconnectAttempt = 0;

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);

//Sensor Vars
uptime Uptime(MQTT_UPTIME, &mqtt);
dimmer ledDimmer(DIMMERPIN, MQTT_DIMMER);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);
#ifdef DEBUG
  debugPort.println("ARDUINO: startup....");
#endif

  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  /*setup wifi*/
  #ifdef DEBUG
  debugPort.println("ARDUINO: setup wifi");
  #endif
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(ssid,password);

#ifdef DEBUG
  debugPort.println("ARDUINO: setup mqtt client");
#endif
  mqttConnect();

  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  
  #ifdef DEBUG
  debugPort.println("ARDUINO: system started");
  #endif
  wdt_enable(WDTO_8S);
  lastMqttReconnectAttempt = 0;
  delay(500);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  wdt_reset();
  esp.process();
  
  if(mqtt.connected())
  {
      Uptime.getUptime();
      Uptime.sendIfChanged();
  }
  else
  {
     #ifdef DEBUG
     Serial.println("Disconnected");
     #endif
     
     long now = millis();
     if (now - lastMqttReconnectAttempt > 3000) 
     {
        lastMqttReconnectAttempt = now;
        // Attempt to reconnect
        if (mqttConnect()) 
        {
           lastMqttReconnectAttempt = 0;
        }
     }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  ledDimmer.setDimmer();
}

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      #ifdef DEBUG
      debugPort.println("WIFI CONNECTED");
      #endif
      mqtt.connect(server, 1883, false);
    } else {
      mqtt.disconnect();
    }
  }
}

void sendMqtt(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  mqtt.publish(topicChar, dataChar);

  #ifdef DEBUG    
  debugPort.println("Sending MQTT");
  debugPort.println(topicChar);
  debugPort.println(dataChar);
  #endif
}

void sendMqtt(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  mqtt.publish(topicChar, dataChar);

  #ifdef DEBUG
  debugPort.println("Sending MQTT");
  debugPort.println(topicChar);
  debugPort.println(dataChar);
  #endif
}

bool mqttConnect()
{
  if (mqtt.begin("Lampy", "admin_lampy", "Isb_C4OGD4c3", 120, 1)) 
  {
    #ifdef DEBUG
    debugPort.println("ARDUINO: setup mqtt lwt");
    #endif
    mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

    /*setup mqtt events */
    mqtt.connectedCb.attach(&mqttConnected);
    mqtt.disconnectedCb.attach(&mqttDisconnected);
    mqtt.publishedCb.attach(&mqttPublished);
    mqtt.dataCb.attach(&mqttData);
  }
  else
  {
    #ifdef DEBUG
    debugPort.println("ARDUINO: fail to setup mqtt");
    #endif
  }
  
  return mqtt.connected();
}

void mqttConnected(void* response)
{
  #ifdef DEBUG
  debugPort.println("MQTT: Connected");
  #endif
    {
    char topicChar[6];
    itoa(MQTT_DIMMER, topicChar, 10);
    mqtt.subscribe(topicChar);
    }
}

void mqttDisconnected(void* response)
{
   #ifdef DEBUG
   debugPort.println("MQTT: Disconeccted");
   #endif
}

void mqttData(void* response)
{
  int topic;
  int data;
  RESPONSE res(response);
  String topicStr = res.popString();
  String dataStr = res.popString();

  #ifdef DEBUG
  debugPort.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  debugPort.print("MQTT: Received: topic=");
  debugPort.println(topicStr);
  debugPort.print("data=");
  debugPort.println(dataStr);
  #endif

  topic = atoi(topicStr.c_str());
  data = atoi(dataStr.c_str());

  switch(topic)
  {
    case MQTT_DIMMER:
    {
      ledDimmer.setValue(data);
    }
    break;
    default:
    break;
  }
}

void mqttPublished(void* response)
{
}

/**************************************************
 *
 *    ftoa - converts float to string
 *
 ***************************************************
 *
 *    This is a simple implemetation with rigid
 *    parameters:
 *            - Buffer must be 8 chars long
 *            - 3 digits precision max
 *            - absolute range is -524,287 to 524,287 
 *            - resolution (epsilon) is 0.125 and
 *              always rounds down
 **************************************************/
 void ftoa(float Value, char* Buffer)
 {
     union
     {
         float f;
     
         struct
         {
             unsigned int    mantissa_lo : 16;
             unsigned int    mantissa_hi : 7;    
             unsigned int     exponent : 8;
             unsigned int     sign : 1;
         };
     } helper;
     
     unsigned long mantissa;
     signed char exponent;
     unsigned int int_part;
     char frac_part[3];
     int i, count = 0;
     
     helper.f = Value;
     //mantissa is LS 23 bits
     mantissa = helper.mantissa_lo;
     mantissa += ((unsigned long) helper.mantissa_hi << 16);
     //add the 24th bit to get 1.mmmm^eeee format
     mantissa += 0x00800000;
     //exponent is biased by 127
     exponent = (signed char) helper.exponent - 127;
     
     //too big to shove into 8 chars
     if (exponent > 18)
     {
         Buffer[0] = 'I';
         Buffer[1] = 'n';
         Buffer[2] = 'f';
         Buffer[3] = '\0';
         return;
     }
     
     //too small to resolve (resolution of 1/8)
     if (exponent < -3)
     {
         Buffer[0] = '0';
         Buffer[1] = '\0';
         return;
     }
     
     count = 0;
     
     //add negative sign (if applicable)
     if (helper.sign)
     {
         Buffer[0] = '-';
         count++;
     }
     
     //get the integer part
     int_part = mantissa >> (23 - exponent);    
     //convert to string
     itoa(int_part, &Buffer[count], 10);
     
     //find the end of the integer
     for (i = 0; i < 8; i++)
         if (Buffer[i] == '\0')
         {
             count = i;
             break;
         }        
 
     //not enough room in the buffer for the frac part    
     if (count > 5)
         return;
     
     //add the decimal point    
     Buffer[count++] = '.';
     
     //use switch to resolve the fractional part
     switch (0x7 & (mantissa  >> (20 - exponent)))
     {
         case 0:
             frac_part[0] = '0';
             frac_part[1] = '0';
             frac_part[2] = '0';
             break;
         case 1:
             frac_part[0] = '1';
             frac_part[1] = '2';
             frac_part[2] = '5';            
             break;
         case 2:
             frac_part[0] = '2';
             frac_part[1] = '5';
             frac_part[2] = '0';            
             break;
         case 3:
             frac_part[0] = '3';
             frac_part[1] = '7';
             frac_part[2] = '5';            
             break;
         case 4:
             frac_part[0] = '5';
             frac_part[1] = '0';
             frac_part[2] = '0';            
             break;
         case 5:
             frac_part[0] = '6';
             frac_part[1] = '2';
             frac_part[2] = '5';            
             break;
         case 6:
             frac_part[0] = '7';
             frac_part[1] = '5';
             frac_part[2] = '0';            
             break;
         case 7:
             frac_part[0] = '8';
             frac_part[1] = '7';
             frac_part[2] = '5';                    
             break;
     }
     
     //add the fractional part to the output string
     for (i = 0; i < 3; i++)
         if (count < 7)
             Buffer[count++] = frac_part[i];
     
     //make sure the output is terminated
     Buffer[count] = '\0';
 }
 
