#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
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

bool utpime_flag   = true;
bool dht22_flag    = true;
bool ds18b20_flag  = true;
bool noise_flag    = true;
bool light_flag    = true;
bool gas_flag      = true;

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xAD, 0xC1, 0xF0, 0xAA, 0xB2 };
byte server[] = { 192, 168, 0, 142 };
byte ip[]     = { 192, 168, 0, 221 };

#ifdef DEBUG
SoftwareSerial debugSerial(DEBUGRXPIN, DEBUGTXPIN); // RX, TX
//HardwareSerial& debugSerial = Serial;
#endif
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);
OneWire oneWire(DS18B20PIN);
DallasTemperature ds18b20(&oneWire);
dht DHT;

//Sensor Vars
uptime Uptime(MQTT_UPTIME);
dimmer ledDimmer(DIMMERPIN);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);

  #ifdef DEBUG
  debugSerial.begin(9600);
  pinMode(DEBUGRXPIN, INPUT);  
  pinMode(DEBUGTXPIN, OUTPUT); 
  debugSerial.println("setup()");
  #endif
  
  ds18b20.begin();
  Ethernet.begin(mac, ip);
  delay(5000);
  mqttConnect();
  
  //Smooth dimming interrupt init
  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  wdt_reset();
  client.loop();
  
  bool motionValue = false;
  bool motion2Value = false;
  int light = map(analogRead(LIGHTPIN), 0, 1024, 100, 0);
  ledDimmer.setCurrentLight(light);

  motionValue  = (bool)digitalRead(MOTIONPIN);
  motion2Value = (bool)digitalRead(MOTION2PIN);
  if (motionValue)  ledDimmer.checkPir();
  if (motion2Value) ledDimmer.checkPir();
  ledDimmer.checkTimeout();

  if(client.connected())
  {
    if (utpime_flag)
    {
      Uptime.getUptime();
      sendMqtt(Uptime.m_mqttTopic, Uptime.m_uptime);
      utpime_flag = false;
    }

    if (dht22_flag)
    { 
      int chk = DHT.read11(DHT11PIN);
      static unsigned short faileCounter = 0;
      if (chk == DHTLIB_OK)
      {
        #ifdef DEBUG
        debugSerial.print("DHT - OK - try: ");
        debugSerial.println(faileCounter);
        #endif
        sendMqtt(MQTT_TEMP_DHT, (float)DHT.temperature);
        sendMqtt(MQTT_HUMIDEX,  (int)DHT.humidity);
        faileCounter = 0;
        dht22_flag = false;
      }
      else
      {
        ++faileCounter;
        if (faileCounter%1000 == 0)
        {
          #ifdef DEBUG
          debugSerial.print("DHT - WARNING NOK - attempt: ");
          debugSerial.println(faileCounter);
          #endif
          dht22_flag = false;
        }
      }
    }

    if (ds18b20_flag)
    {
      ds18b20.requestTemperatures();
      sendMqtt(MQTT_TEMP_DS18, ds18b20.getTempCByIndex(0));
      ds18b20_flag = false;
    }

    static bool lastMotionValue = false;
    if (lastMotionValue != motionValue)
    {
      sendMqtt(MQTT_MOTION, (int)motionValue);
      lastMotionValue = motionValue;
    }
    
    static bool lastMotion2Value = false;
    if (lastMotion2Value != motion2Value)
    {
      sendMqtt(MQTT_MOTION2, (int)motion2Value);
      lastMotion2Value = motion2Value;
    }

    if (noise_flag)
    {
    //  sendMqtt(MQTT_NOISE ,(int)map(analogRead(NOISEPIN), 0, 1024, 100, 0));
    //  noise_flag = false;
    }

    if (light_flag)
    {
      sendMqtt(MQTT_LIGHT, light);
      light_flag = false;
    }

    if (gas_flag)
    {
      sendMqtt(MQTT_GAS , (int)map(analogRead(GASPIN), 10, 1024, 0, 100));
      gas_flag = false;
    }
  }
  else
  {
     #ifdef DEBUG
     debugSerial.println("Disconnected");
     #endif
     mqttConnect();
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  if (dht22_flag)
  {
    return;
  }
  
  static long counter = 0;
  ++counter;

  if (counter%(UPTIMEPERIOD*COUNTIN1SEC) == 0)
  {
    utpime_flag = true;
  }

  if (counter%(TEMPPERIOD*COUNTIN1SEC) == 0)
  {
    dht22_flag = true;
    ds18b20_flag = true;
  }

  if (counter%(NOISEPERIOD*COUNTIN1SEC) == 0)
  {
    noise_flag = true;
  }

  if (counter%(LIGHTPERIOD*COUNTIN1SEC) == 0)
  {
    light_flag = true;
  }

  if (counter%(GASPERIOD*COUNTIN1SEC) == 0)
  {
    gas_flag = true;
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
  debugSerial.println("Receiving MQTT");
  debugSerial.print(topic);
  debugSerial.print(" => ");
  debugSerial.write(payload, length);
  debugSerial.println();
  #endif
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = atoi((char *) p);

  switch(atoi(topic))
  {
    case MQTT_DIMMER:
    {
      ledDimmer.setValue(data);
    }
      break;
    case MQTT_DIMMER_TRIGGER:
    {
      ledDimmer.setTrigger(data);
    }
      break;
    case MQTT_DIMMER_TIMEOUT:
    {
      ledDimmer.setTimeout(data);
    }
      break;
    case MQTT_DIMMER_PIR:
    {
      if (data) 
      {
        ledDimmer.setPirFlag(true);
      }
      else 
      {
        ledDimmer.setPirFlag(false);
      }
    }
      break;
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

  #ifdef DEBUG    
  debugSerial.println("Sending MQTT");
  debugSerial.println(topicChar);
  debugSerial.println(dataChar);
  #endif
}

void sendMqtt(int topic, float value)
{
  char topicChar[6];
  char dataChar[8];
  itoa(topic, topicChar, 10);
  ftoa(value, dataChar);
  client.publish(topicChar, dataChar);

  #ifdef DEBUG
  debugSerial.println("Sending MQTT");
  debugSerial.println(topicChar);
  debugSerial.println(dataChar);
  #endif
}

void mqttConnect()
{
  if (client.connect("DuzyPokoj", "admin", "Isb_C4OGD4c3")) 
  {
    #ifdef DEBUG
    debugSerial.println("Connected");
    #endif
    delay(5000);
    char topicChar[6];
    itoa(MQTT_DIMMER, topicChar, 10);
    client.subscribe(topicChar);
    itoa(MQTT_DIMMER_TIMEOUT, topicChar, 10);
    client.subscribe(topicChar);
    itoa(MQTT_DIMMER_TRIGGER, topicChar, 10);
    client.subscribe(topicChar);
    itoa(MQTT_DIMMER_PIR, topicChar, 10);
    client.subscribe(topicChar);
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
 
