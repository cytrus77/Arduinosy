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
bool wind_measure_flag = true;
bool wind_flag     = false;
bool rain_flag     = true;
bool dht22_flag    = true;
bool ds18b20_flag  = true;
bool noise_flag    = true;
bool light_flag    = true;
bool pressure_flag = true;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xAD, 0xC1, 0xF0, 0x00, 0x01 };
byte server[] = { 192, 168, 0, 142 };
byte ip[]     = { 192, 168, 0, 220 };

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
dimmer ledDimmer(MQTT_DIMMER, DIMMERPIN);
float windBuffer[10];
short windBufferCounter = 0;
float windSpeed = 0;

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);

  #ifdef DEBUG
  debugSerial.begin(9600);
  pinMode(DEBUGRXPIN, INPUT);  
  pinMode(DEBUGTXPIN, OUTPUT); 
  debugSerial.println("setup()");
  #endif
  
  pinMode(RS485TXENPIN, OUTPUT);  
  pinMode(RS485RXENPIN, OUTPUT);
  digitalWrite(RS485TXENPIN, RS485TransmitOff); 
  digitalWrite(RS485RXENPIN, RS485ReceiveOff); 

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

  if(client.connected())
  {
    if (utpime_flag)
    {
      Uptime.getUptime();
      sendMqtt(Uptime.m_mqttTopic, Uptime.m_uptime);
      utpime_flag = false;
    }

    if (wind_measure_flag)
    {
      sendAnemometerRequest();
      getWindValue();
      wind_measure_flag = false;
    }
    
    if (wind_flag)
    {
      if (windSpeed >= 0)
      {
        sendMqtt(MQTT_WIND , windSpeed);
      }
      wind_flag = false;
    }

    if (rain_flag)
    {
      sendMqtt(MQTT_RAIN , (int)map(analogRead(RAINPIN), 10, 1024, 100, 0));
      rain_flag = false;
    }

    if (dht22_flag)
    {
      int chk = DHT.read22(DHT22PIN);
      static unsigned short faileCounter = 0;
      if (chk == DHTLIB_OK)
      {
        #ifdef DEBUG
        debugSerial.println("DHT - OK");
        #endif
        sendMqtt(MQTT_TEMP_DHT, (float)DHT.temperature);
        sendMqtt(MQTT_HUMIDEX,  (int)DHT.humidity);
        faileCounter = 0;
        dht22_flag = false;
      }
      else
      {
        #ifdef DEBUG
        ++faileCounter;
        if (faileCounter%1000 == 0)
        {
          debugSerial.print("DHT - WARNING NOK - attempt: ");
          debugSerial.println(faileCounter);
          dht22_flag = false;
        }
        #endif
      }
    }

    if (ds18b20_flag)
    {
      ds18b20.requestTemperatures();
      sendMqtt(MQTT_TEMP_DS18, ds18b20.getTempCByIndex(0));
      ds18b20_flag = false;
    }

    if (noise_flag)
    {
    //  sendMqtt(MQTT_NOISE ,(int)map(analogRead(NOISEPIN), 0, 1024, 100, 0));
    //  noise_flag = false;
    }

    if (light_flag)
    {
      sendMqtt(MQTT_LIGHT , (int)map(analogRead(LIGHTPIN), 0, 1024, 100, 0));
      light_flag = false;
    }

    if (pressure_flag)
    {
      
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

  if (counter%(WINDPERIOD*COUNTIN1SEC/10) == 0)
  {
    wind_measure_flag = true;
  }
  
  if (counter%(WINDPERIOD*COUNTIN1SEC) == 0)
  {
    wind_flag = true;
  }

  if (counter%(RAINDPERIOD*COUNTIN1SEC) == 0)
  {
    rain_flag = true;
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

  if (counter%(PRESSUREPERIOD*COUNTIN1SEC) == 0)
  {
    pressure_flag = true;
  }
  
/*  static int pirLastValue = 0;
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
  ledDimmer.setDimmer();*/
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  
  #ifdef DEBUG
  debugSerial.print(topic);
  debugSerial.print(" => ");
  debugSerial.write(payload, length);
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
      break;
    }
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
  if (client.connect("DVES_duino", "admin", "Isb_C4OGD4c3")) 
  {
    #ifdef DEBUG
    debugSerial.println("Connected");
    #endif
    delay(5000);
    char topicChar[6];
    itoa(MQTT_DIMMER, topicChar, 10);
    client.subscribe(topicChar);
  }
}

void sendAnemometerRequest()
{
    digitalWrite(RS485TXENPIN, RS485TransmitOn); 
    digitalWrite(RS485RXENPIN, RS485ReceiveOff); 
    Serial.write((uint8_t)0x02);
    Serial.write((uint8_t)0x03);
    Serial.write((uint8_t)0x00);
    Serial.write((uint8_t)0x00); 
    Serial.write((uint8_t)0x00);
    Serial.write((uint8_t)0x01);
    Serial.write((uint8_t)0x84);
    Serial.write((uint8_t)0x39);
}

void getWindValue()
{
    int tempBuffer[10];
    short buffeIt = 0;
    float tempWindSpeed = -1;
    digitalWrite(RS485TXENPIN, RS485TransmitOff); 
    digitalWrite(RS485RXENPIN, RS485ReceiveOn); 
    
    if(Serial.available())
    {
      while( Serial.available()) 
      {
        tempBuffer[buffeIt] = Serial.read();
        ++buffeIt;
        if (buffeIt >= 7 && 
            tempBuffer[buffeIt-6] == 0x02 && 
            tempBuffer[buffeIt-5] == 0x03 && 
            tempBuffer[buffeIt-4] == 0x02)
        {
          windBuffer[windBufferCounter] = (float)(tempBuffer[buffeIt-3] * 0xFF + tempBuffer[buffeIt-2])/10;
          ++windBufferCounter;
          float sum = 0;
          for (int i=0; i<10; ++i)
          {
            sum += windBuffer[i];
          }
          windSpeed = sum/10;
          #ifdef DEBUG
          debugSerial.print("Wind speed = ");
          debugSerial.println(windSpeed);
          #endif
          break;
        }
      }
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
 
