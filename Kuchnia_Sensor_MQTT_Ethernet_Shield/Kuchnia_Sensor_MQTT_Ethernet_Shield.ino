#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht11.h>
#include <string.h>
#include "TimerOne.h"

#define DEBUG 1

#define mq2Pin A5
#define pirPin 3
#define photoPin A4
#define dht11Pin 4
#define relayPin 5
#define keyPin 6
#define mqttDiodaPin 7
#define gasDiodaPin 8
#define floodPin A3

#define Light_Time 120   //czas swiecenia sie swiatla w sekundach

//DHT11 Vars
dht11 DHT11;
signed int temperatura;
unsigned int wilgotnosc;

//PIR Vars
boolean motion = false; 

//Photo Vars
int photo = 0;

//Gas vars
int gas = 0;

//Flood var
int flood = 0;

//Dimmer
int dimmer = 0;
int currentDimmer = 0;

long int uptime = 0;
boolean mqtt_status = false;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xEF };
byte server[] = { 192, 168, 137, 2 };
byte ip[]     = { 192, 168, 137, 100 };

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  
  //#ifdef DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  //#endif
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = atoi((char *) p);

  switch(atoi(topic))
  {
    case 1010:
    {
      digitalWrite(relayPin,data);
      break;
    }
    case 1020:
    {
      dimmer = data;    
      break;
    }
    default:
    break;
  }
  // Free the memory
  free(p);
}

void setup()
{
  //#ifdef DEBUG
  Serial.begin(9600);
  //#endif
  
  //PIN Config section
  pinMode(pirPin, INPUT);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, 0);
  
  pinMode(mqttDiodaPin, OUTPUT);
  digitalWrite(mqttDiodaPin, 0);
  
  pinMode(gasDiodaPin, OUTPUT);
  digitalWrite(gasDiodaPin, 0);
  
  pinMode(keyPin, OUTPUT);
  analogWrite(keyPin, 0);
  
  Ethernet.begin(mac, ip);
  delay(10000);
  if (client.connect("KuchniaClient")) {
    delay(1000);
    client.subscribe("1010");
    client.subscribe("1020");
  }
}


void loop()
{
  client.loop();
  
  Dimmer1();
  DHT11Pomiar(); 
  PirPomiar();
  PhotoPomiar();
  FloodPomiar();
  Mq2Pomiar();
  
  if(client.connected()){ 
     static long int uptime_temp = 0;
     uptime = millis()/60000;
     if(uptime_temp != uptime)
     {
         SendMQTT(uptime,"1000");
         uptime_temp = uptime;
         delay(150);
     }        
    digitalWrite(mqttDiodaPin,HIGH);
    mqtt_status = true;
  }
  else
  {    
    digitalWrite(mqttDiodaPin,LOW);
    mqtt_status = false;
    static long int PIR_Timer = 0;
       
    if(motion==1) 
    {
      PIR_Timer = millis();
      dimmer = 100;
    }
    else if((millis()-PIR_Timer)/1000 > Light_Time) 
    {
      dimmer = 0;
    }
        
    #ifdef DEBUG
    Serial.println("PDALO POLACZENIE Z MQTT !!!!!!!!");
    #endif
    
    if(client.connect("arduinoClient"))
    {    
        #ifdef DEBUG
        Serial.println("Connected to MQTT server");
        #endif
        client.subscribe("1010");
        client.subscribe("1020");
        //client.subscribe("test");
    }
  }
}



void Dimmer1()
{
  static int dimmer_temp = 0;
  if(dimmer != dimmer_temp)
  {
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(DimmerInt);
  dimmer_temp = dimmer;
  #ifdef DEBUG
  Serial.println("Timer 1 wlaczony!!!!!!");
  #endif
  }
}


void DimmerInt(){
  #ifdef DEBUG
  Serial.println("Przerwanie Timer 1 - Dimmer");
  #endif
  DimmerSet(keyPin, &dimmer);
}


void DimmerSet(int pin, int* dimmer){
  static int tempDim = 0;
  
  if(currentDimmer < *dimmer)
  {
    tempDim++;
  }
  else if(currentDimmer > *dimmer)
  {
    tempDim--;
  }
  currentDimmer = map(tempDim, 0, 255, 0, 100);
  analogWrite(pin, tempDim*tempDim/255);   //kwadratowo
  if(currentDimmer == *dimmer) 
  {
    #ifdef DEBUG
    Serial.println("Timer 1 wylaczony");
    #endif
    Timer1.detachInterrupt();
  }
}


void SendMQTT(int data, char* topic){
  char charBuf[6];
  itoa(data, charBuf, 10);
  client.publish(topic, charBuf);
  
  #ifdef DEBUG
  Serial.println(charBuf);
  #endif
}


void Mq2Pomiar(){
  static int gas_temp;
  
  gas = analogRead(mq2Pin);
  gas = map(gas,0,1024,0,100);
  
          
  if(gas_temp != gas && mqtt_status == true)
  {
     SendMQTT(gas,"1091");
     gas_temp = gas;
     delay(150);
  }
  
  #ifdef DEBUG
  Serial.print("Gaz: ");
  Serial.println(gas);
  #endif
  
  if(gas > 25) 
  {
    digitalWrite(gasDiodaPin, HIGH);
    #ifdef DEBUG
    Serial.println("UWAGA - WYKRYTO GAZ LUB DYM");
    #endif
  }
  else
  {
    digitalWrite(gasDiodaPin, LOW);
  }
}


void FloodPomiar() {
  static int flood_temp;
  
  flood = analogRead(floodPin);
  flood = map(flood,0,1024,100,0);
  
  if(flood_temp != flood && mqtt_status == true)
  {
    SendMQTT(flood,"1092");
    flood_temp = flood;
    delay(150);
  }
  
  #ifdef DEBUG
  Serial.print("Zalanie: ");
  Serial.println(flood);
  #endif
}


void PhotoPomiar() {
  static int photo_temp;
  photo = analogRead(photoPin);
  photo = map(photo,0,1024,100,0);
     
  if(photo_temp != photo && mqtt_status == true)
  {
     SendMQTT(photo,"1070");
     photo_temp = photo;
     delay(150);
  }
  
  #ifdef DEBUG
  Serial.print("Swiatlo: ");
  Serial.println(photo);
  #endif
}


void DHT11Pomiar(){
  switch (DHT11.read(dht11Pin))
  {
    case DHTLIB_OK: 
                static signed int temperatura_temp;
                static unsigned int wilgotnosc_temp;
                
                #ifdef DEBUG
                Serial.println("\n");
                Serial.print("DHT 11 Read sensor: ");
                #endif
                
                wilgotnosc = (int)DHT11.humidity;
                temperatura = (int)DHT11.temperature;
                
                #ifdef DEBUG
                Serial.print("Humidity (%): ");
                Serial.println((float)DHT11.humidity, 2);
              
                Serial.print("Temperature (Â°C): ");
                Serial.println((float)DHT11.temperature, 2);
                #endif
                
                if(temperatura_temp != temperatura && mqtt_status == true)
                {
                    SendMQTT(temperatura,"1030");
                    temperatura_temp = temperatura;
                    delay(150);
                }
                if(wilgotnosc_temp != wilgotnosc && mqtt_status == true)
                {
                    SendMQTT(wilgotnosc,"1090");
                    wilgotnosc_temp = wilgotnosc;
                    delay(150);
                }
                
		//Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//Serial.println("Time out error"); 
		break;
    default: 
		//Serial.println("Unknown error"); 
		break;
  }
}


void PirPomiar (){
  static boolean motion_temp; 
  
   if(digitalRead(pirPin) == HIGH){
        motion = true;
       }
   else if(digitalRead(pirPin) == LOW){       
        motion = false;
       } 
   
   if(motion_temp != motion && mqtt_status == true)
   {
      SendMQTT(motion,"1040");
      motion_temp = motion;
      delay(150);
   }    
   
   #ifdef DEBUG
   Serial.print("Ruch: ");
   Serial.println(motion);
   #endif  
}

