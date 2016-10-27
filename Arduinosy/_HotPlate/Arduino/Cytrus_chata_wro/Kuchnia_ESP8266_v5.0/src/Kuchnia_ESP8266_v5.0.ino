#include <Arduino.h>

/**
 * \file
 *       ESP8266 MQTT Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */
//#include <SPI.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include "TimerOne.h"
#include "avr/wdt.h"
#include <mqtt.h>
#include <string.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>

#include "Defines.h"
#include "DimmerPir.h"
#include "Uptime.h"


void ftoa(float Value, char* Buffer);

bool utpime_flag   = true;
bool dht22_flag    = true;
bool ds18b20_flag  = true;
bool light_flag    = true;
bool gas_flag      = true;
bool mqttStatus    = false;

const char* ssid = "cytrynowa_wro";
const char* password = "limonkowy";
char server[] = "192.168.0.142";
long lastMqttReconnectAttempt = 0;

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);
OneWire oneWire(DS18B20PIN);
DallasTemperature ds18b20(&oneWire);
dht DHT;

//Sensor Vars
uptime Uptime(MQTT_UPTIME, &mqtt);
dimmerPir ledDimmer(DIMMERPIN);

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

//  pinMode(DEBUGRXPIN, INPUT);
//  pinMode(DEBUGTXPIN, OUTPUT);
//  debugPort.println("setup()");

//  ds18b20.begin();
//  delay(500);
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

  Timer1.initialize(TIMER0PERIOD);
  Timer1.attachInterrupt(TimerLoop);

  #ifdef DEBUG
  debugPort.println("ARDUINO: system started");
  #endif
  wdt_enable(WDTO_8S);
  delay(3000);
  lastMqttReconnectAttempt = 0;
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////
      long dht11time = 0;
/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {
  wdt_reset();
  esp.process();

  bool motionValue = false;
  bool motion2Value = false;
  int light = map(analogRead(LIGHTPIN), 0, 1024, 100, 0);
  ledDimmer.setCurrentLight(light);

  motionValue  = (bool)digitalRead(MOTIONPIN);
  motion2Value = (bool)digitalRead(MOTION2PIN);
  if (motionValue)  ledDimmer.checkPir();
  if (motion2Value) ledDimmer.checkPir();
  ledDimmer.checkTimeout();

  if(mqttStatus)
  {
    if (utpime_flag)
    {
      Uptime.getUptime();
      sendMqtt(Uptime.m_mqttTopic, Uptime.m_uptime);
      utpime_flag = false;
    }

    if (dht22_flag)
    {
      long start = millis();
      int chk = DHT.read11(DHT11PIN);
      static unsigned short faileCounter = 0;
      if (chk == DHTLIB_OK)
      {
        #ifdef DEBUG
        debugPort.print("DHT - OK - try: ");
        debugPort.println(faileCounter);
        #endif
        sendMqtt(MQTT_TEMP_DHT, (float)DHT.temperature);
        sendMqtt(MQTT_HUMIDEX,  (int)DHT.humidity);
        faileCounter = 0;
        dht22_flag = false;
        long end_time = millis();
        dht11time = end_time - start;
        {
        debugPort.print("DHT11 time = ");
        debugPort.print(dht11time);
        debugPort.println("ms");
        }
      }
      else
      {
        ++faileCounter;
        if (faileCounter%1000 == 0)
        {
          #ifdef DEBUG
          debugPort.print("DHT - WARNING NOK - attempt: ");
          debugPort.println(faileCounter);
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
     long now = millis();
     if (now - lastMqttReconnectAttempt > 3000)
     {
        #ifdef DEBUG
        debugPort.println("Disconnected");
        #endif
        lastMqttReconnectAttempt = now;
        // Attempt to reconnect
        mqttConnect();
     }
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

void mqttConnect()
{
  if (mqtt.begin("DVES_duino1", "admin", "Isb_C4OGD4c3", 120, 1))
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
}

void mqttConnected(void* response)
{
  #ifdef DEBUG
  debugPort.println("MQTT: Connected");
  #endif
  mqttStatus = true;
    {
    char topicChar[6];
    itoa(MQTT_DIMMER, topicChar, 10);
    mqtt.subscribe(topicChar);
    itoa(MQTT_DIMMER_TIMEOUT, topicChar, 10);
    mqtt.subscribe(topicChar);
    itoa(MQTT_DIMMER_TRIGGER, topicChar, 10);
    mqtt.subscribe(topicChar);
    itoa(MQTT_DIMMER_PIR, topicChar, 10);
    mqtt.subscribe(topicChar);
    }
}

void mqttDisconnected(void* response)
{
   #ifdef DEBUG
   debugPort.println("MQTT: Disconeccted");
   #endif
   mqttStatus = false;
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
    case MQTT_DIMMER_TRIGGER:
    {
      ledDimmer.setTrigger(data);
    }
    break;
    case MQTT_DIMMER_TIMEOUT:
    {
      ledDimmer.setTimeout(data);
    }
    case MQTT_DIMMER_PIR:
    {
      if (data) ledDimmer.setPirFlag(true);
      else ledDimmer.setPirFlag(false);
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
