#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <string.h>
#include <avr/wdt.h>
#include "TimerOne.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>

#include "Defines.h"
#include "Dimmer.h"
#include "Uptime.h"

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);


// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0x33, 0x1A, 0xFE, 0x11, 0xEF };
byte server[] = { 192, 168, 0, 142 };
byte ip[]     = { 192, 168, 0, 222 };

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
dht DHT;

//Sensor Vars
dimmer ledDimmer(MQTT_DIMMER, LEDDIMMERPIN, MQTT_DIMMER_TIMER, MQTT_PHOTO_TRIGGER, MQTT_DIMMER_STATUS, MQTT_DIMMER_TIMER_STATUS, MQTT_PHOTO_TRIGGER_STATUS);
uptime Uptime(MQTT_UPTIME);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  
  #ifdef DEBUG
  Serial.println("setup()");
  #endif
  
  ds18b20.begin();
  Ethernet.begin(mac, ip);
  delay(5000);
  mqttConnect();
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_8S);

  pinMode(MOTIONSENSORPIN, INPUT);
  pinMode(MOTIONSENSOR2PIN, INPUT);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  wdt_reset();
  client.loop();

  if(client.connected())
  {
    Uptime.getUptime();
    Uptime.sendIfChanged();

    ds18b20.requestTemperatures();
    sendMqtt(MQTT_TEMP_DS18, ds18b20.getTempCByIndex(0));

    // READ DATA
    int chk = DHT.read22(DHT22PIN);
    if (chk == DHTLIB_OK)
    {
      Serial.println("DHT - OK"); 
      sendMqtt(MQTT_TEMP_DHT, (float)DHT.temperature);
      sendMqtt(MQTT_HUMIDEX,  (int)DHT.humidity);
    }

    sendMqtt(MQTT_MOTION ,digitalRead(MOTIONSENSORPIN));
    sendMqtt(MQTT_MOTION2 ,digitalRead(MOTIONSENSOR2PIN));
  }
  else
  {
     Serial.println("Rozlaczony");
     mqttConnect();
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  static int pirLastValue = 0;
  if(pirLastValue)
  {
    if(ledDimmer.m_trigger)
    {
      if(ledDimmer.m_setValue > 25)
      {
        ledDimmer.resetTimer();
      }
      else
      {
        ledDimmer.setValue(100);
      }
    }
  }
  ledDimmer.setDimmer();
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  
  #ifdef DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  #endif
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = atoi((char *) p);

  switch(atoi(topic))
  {
/*    case MQTT_RELAY:
    {
 //     mqttBuffer[MQTT_RELAY_NO].Data  = data;
      break;
    }
    case MQTT_DIMMER:
    {  
  //    mqttBuffer[MQTT_DIMMER_NO].Data = data;
      break;
    }*/
    default:
    break;
  }
  // Free the memory
  free(p);
}

void sendMqtt(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  client.publish(topicChar, dataChar);
      
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}

void sendMqtt(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  client.publish(topicChar, dataChar);
      
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}

void mqttConnect()
{
  if (client.connect("DVES_duino", "admin", "Isb_C4OGD4c3")) 
  {
    Serial.println("Connected");
    delay(5000);
    client.subscribe("inTopic");
  }
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
 
