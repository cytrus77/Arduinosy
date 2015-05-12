#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht11.h>
#include <string.h>

//#define DEBUG 1

#define mq2Pin A5
#define pirPin 3
#define photoPin A4
#define dht11Pin 4
#define relayPin 5
#define keyPin 6
#define diodaPin 7

#define Light_Time 10   //czas swiecenia sie swiatla w sekundach

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
      Serial.println("Przekaznik");
      break;
    }
    case 1020:
    {
      analogWrite(keyPin, map(data,0,100,0,255));
      Serial.println("Dimmer");
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
  //attachInterrupt(pirPin, PirPomiar, CHANGE);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, 0);
  
  pinMode(diodaPin, OUTPUT);
  digitalWrite(diodaPin, 0);
  
  pinMode(keyPin, OUTPUT);
  analogWrite(keyPin, 0);
  
  Ethernet.begin(mac, ip);
  if (client.connect("KuchniaClient")) {
    delay(10000);
    //client.publish("outTopic","hello world");
    client.subscribe("1010");
    client.subscribe("1020");
  }
  
  
}

void loop()
{
  client.loop();
  DHT11Pomiar();
   
  PirPomiar();
  
  PhotoPomiar();
  
  Mq2Pomiar();
  
  if(client.connected()){ 
    static int mqtt_counter = 0;
       
    switch(mqtt_counter)
    {
      case 0:
        SendMQTT(temperatura,"1030");
        mqtt_counter++;
        break;
      case 1:   
        SendMQTT(wilgotnosc,"1090");
        mqtt_counter++;
        break;
      case 2:
        SendMQTT(photo,"1070");
        mqtt_counter++;
        break;
      case 3:
        SendMQTT(motion,"1040");
        mqtt_counter++;
        break;
      case 4:
        SendMQTT(gas,"1091");
        mqtt_counter++;
        break;
      case 5:
        SendMQTT(millis()/60000,"1000");
        mqtt_counter++;
        break;
      default:
        //mqtt_counter++;
        // if(mqtt_counter > 50) 
        mqtt_counter = 0;
        break; 
    }
        
    digitalWrite(diodaPin,HIGH);
  }
  else
  {    
    digitalWrite(diodaPin,LOW);
    
    static long int PIR_Timer = 0;
    
    if(motion==1) 
    {
      PIR_Timer = millis();
      analogWrite(keyPin, 255);
    }
    else if((millis()-PIR_Timer)/1000 > Light_Time) 
    {
      analogWrite(keyPin, 0);
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
  delay(250);
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
  gas = analogRead(mq2Pin);
  gas = map(gas,0,1024,0,100);
  
  #ifdef DEBUG
  Serial.print("Gaz: ");
  Serial.println(gas);
  #endif
  
  if(gas > 30) 
  {
    #ifdef DEBUG
    Serial.println("UWAGA - WYKRYTO GAZ LUB DYM");
    #endif
  }
}


void PhotoPomiar() {
  photo = analogRead(photoPin);
  photo = map(photo,0,1024,0,100);
  
  #ifdef DEBUG
  Serial.print("Swiatlo: ");
  Serial.println(photo);
  #endif
}


void DHT11Pomiar(){
  switch (DHT11.read(dht11Pin))
  {
    case DHTLIB_OK: 
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
   if(digitalRead(pirPin) == HIGH){
        motion = true;
       }
   else if(digitalRead(pirPin) == LOW){       
        motion = false;
       } 
   #ifdef DEBUG
   Serial.print("Ruch: ");
   Serial.println(motion);
   #endif  
   Serial.println("Ruch: ");
   Serial.println(motion);
}

