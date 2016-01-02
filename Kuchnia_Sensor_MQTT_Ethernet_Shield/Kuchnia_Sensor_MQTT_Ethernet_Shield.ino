#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <dht11.h>
#include <string.h>
#include "TimerOne.h"
#include "Kuchnia.h"
#include <avr/wdt.h>

#define DEBUG 1
#define LOG 1

void mqttConfig()
{
  mqttBuffer[MQTT_UPTIME_NO].Topic = MQTT_UPTIME;
  mqttBuffer[MQTT_RELAY_NO].Topic  = MQTT_RELAY;
  mqttBuffer[MQTT_DIMMER_NO].Topic = MQTT_DIMMER;
  mqttBuffer[MQTT_GAS_NO].Topic    = MQTT_GAS;
  mqttBuffer[MQTT_FLOOD_NO].Topic  = MQTT_FLOOD;
  mqttBuffer[MQTT_PHOTO_NO].Topic  = MQTT_PHOTO;
  mqttBuffer[MQTT_TEMP_NO].Topic   = MQTT_TEMP;
  mqttBuffer[MQTT_HUMI_NO].Topic   = MQTT_HUMI;
  mqttBuffer[MQTT_MOTION_NO].Topic = MQTT_MOTION;
}

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
    case MQTT_RELAY:
    {
      mqttBuffer[MQTT_RELAY_NO].Data  = data;
      break;
    }
    case MQTT_DIMMER:
    {  
      mqttBuffer[MQTT_DIMMER_NO].Data = data;
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
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  #ifdef LOG
  Serial.begin(115200);
  #endif
  
  //PIN Config section
  pinMode(PIRPIN, INPUT);
  //attachInterrupt(0, PirPomiar, CHANGE);
  
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, 0);
  
  pinMode(MQTTDIODAPIN, OUTPUT);
  digitalWrite(MQTTDIODAPIN, 0);
  
  pinMode(GASDIODAPIN, OUTPUT);
  digitalWrite(GASDIODAPIN, 0);
  
  pinMode(KEYPIN, OUTPUT);
  analogWrite(KEYPIN, 0);
  
  Ethernet.begin(mac, ip);
  delay(5000);
  if (client.connect("KuchniaClient")) {
    delay(5000);
    client.subscribe("1010");
    client.subscribe("1020");
  }
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  #ifdef DEBUG
  Serial.println("Timer 1 wlaczony!!!!!!");
  #endif
  
  mqttConfig();
  wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////


void loop()
{
  static unsigned int mainCounter = 0; 
  client.loop();
  wdt_reset();
  PhotoPomiar();
  FloodPomiar();
  Mq2Pomiar();
  mainCounter++;
  if(client.connected()){ 
    mqttBuffer[MQTT_UPTIME_NO].Data = millis()/60000;
       
    digitalWrite(MQTTDIODAPIN,HIGH);
    mqtt_status = true;
    
    static boolean lastPir = false;
    if(lastPir != mqttBuffer[MQTT_MOTION_NO].Data)
       {
           char dataChar[6];
           char topicChar[6];
           itoa(mqttBuffer[MQTT_MOTION_NO].Data, dataChar, 10);
           itoa(mqttBuffer[MQTT_MOTION_NO].Topic, topicChar, 10);
           client.publish(topicChar, dataChar);
           #ifdef DEBUG
           Serial.println("Sending MQTT");
           Serial.println(topicChar);
           Serial.println(dataChar);
           #endif 
           lastPir = mqttBuffer[MQTT_MOTION_NO].Data;
           mainCounter = 1;
       }

    
    if(!(mainCounter%2000)){
        #ifdef LOG
        Serial.print("Main counter: ");
        Serial.println(mainCounter);
        #endif
        #ifdef DEBUG
        Serial.println("Przerwanie Timer 1 - SendMQTT");
        Serial.print("Przerwanie Timer 1 - Loop counter");
        Serial.println(mainCounter);
        #endif
        SendMqtt();
    }
    
    if(!(mainCounter%5000)){
        #ifdef DEBUG
        Serial.println("Przerwanie Timer 1 - DHT11");
        #endif
        DHT11Pomiar(); 
    }
  }
  else
  {    
    digitalWrite(MQTTDIODAPIN,LOW);
    mqtt_status = false;
    static long int PIR_Timer = 0;
    static boolean auto_flag = false;
    if(mqttBuffer[MQTT_MOTION_NO].Data==1 && mqttBuffer[MQTT_PHOTO_NO].Data < 45) 
    {
      PIR_Timer = millis();
      mqttBuffer[MQTT_DIMMER_NO].Data = 100;
      auto_flag == true;
    }
    else if(mqttBuffer[MQTT_MOTION_NO].Data==1 && auto_flag == true)
    {
      PIR_Timer = millis();
    }
    else if((millis()-PIR_Timer)/1000 > LIGHT_TIME) 
    {
      mqttBuffer[MQTT_DIMMER_NO].Data = 0;
      auto_flag == false;
    }
    #ifdef LOG
    Serial.println("PDALO POLACZENIE Z MQTT !!!!!!!!");
    #endif    
    #ifdef DEBUG
    Serial.println("PDALO POLACZENIE Z MQTT !!!!!!!!");
    #endif
    if(1)
    {
        static int mqttFails = 0;
        #ifdef DEBUG
         Serial.println("Recovery action dla MQTT");
        #endif
        ethClient.stop();
        if(client.connect("KuchniaClient"))
        {            
            #ifdef DEBUG
            Serial.println("Connected to MQTT server");
            #endif
            client.subscribe("1010");
            client.subscribe("1020");
            //client.subscribe("test");
            mqttFails = 0;
        }
        else
        {
            mqttFails++;
            #ifdef DEBUG
            Serial.print("Increment mqtt fail counter :");
            Serial.println(mqttFails);
            #endif
            ethClient.stop();
            if(mqttFails > 25)
            {
                #ifdef LOG
                Serial.println("WDG_RESET!!!!!!!!!!");
                #endif    
                #ifdef DEBUG
                Serial.println("WDG_RESET!!!!!!!!!!");
                #endif  
                wdt_enable(WDTO_15MS);
                while(1){}
            }
        }
    }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////


void TimerLoop(){
  static unsigned int loopCounter = 0;
  loopCounter++;
  DimmerSet(KEYPIN, &mqttBuffer[MQTT_DIMMER_NO].Data);
  
  if(!(loopCounter%100)) PirPomiar();
}


void DimmerSet(int pin, int* dimmer){
  static int tempDim = 0;
  if(currentDimmer != *dimmer)
  {
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
    #ifdef DEBUG
    Serial.println("Przerwanie Timer 1 - Dimmer");
    #endif
  }
}


void SendMqtt(){
  static int counter = 0;
  counter = (counter + 1)%7;
  char dataChar[6];
  char topicChar[6];
  itoa(mqttBuffer[counter].Data, dataChar, 10);
  itoa(mqttBuffer[counter].Topic, topicChar, 10);
  client.publish(topicChar, dataChar);
      
  #ifdef LOG
  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
  #endif
}


void Mq2Pomiar(){
  mqttBuffer[MQTT_GAS_NO].Data = analogRead(MQ2PIN);
  mqttBuffer[MQTT_GAS_NO].Data = map(mqttBuffer[MQTT_GAS_NO].Data,0,1024,0,100);
  
  #ifdef DEBUG
  Serial.print("Gaz: ");
  Serial.println(mqttBuffer[MQTT_GAS_NO].Data);
  #endif
  
  if(mqttBuffer[MQTT_GAS_NO].Data > 25) 
  {
    digitalWrite(GASDIODAPIN, HIGH);
    #ifdef DEBUG
    Serial.println("UWAGA - WYKRYTO GAZ LUB DYM");
    #endif
  }
  else
  {
    digitalWrite(GASDIODAPIN, LOW);
  }
}


void FloodPomiar() {
  mqttBuffer[MQTT_FLOOD_NO].Data = analogRead(FLOODPIN);
  mqttBuffer[MQTT_FLOOD_NO].Data = map(mqttBuffer[MQTT_FLOOD_NO].Data,0,1024,100,0);

  #ifdef DEBUG
  Serial.print("Zalanie: ");
  Serial.println(mqttBuffer[MQTT_FLOOD_NO].Data);
  #endif
}


void PhotoPomiar() {
  mqttBuffer[MQTT_PHOTO_NO].Data = analogRead(PHOTOPIN);
  mqttBuffer[MQTT_PHOTO_NO].Data = map(mqttBuffer[MQTT_PHOTO_NO].Data,0,1024,100,0);
  
  #ifdef DEBUG
  Serial.print("Swiatlo: ");
  Serial.println(mqttBuffer[MQTT_PHOTO_NO].Data);
  #endif
}


void DHT11Pomiar(){
  switch (DHT11.read(DHT11PIN))
  {
    case DHTLIB_OK:              
                #ifdef DEBUG
                Serial.println("\n");
                Serial.print("DHT 11 Read sensor: ");
                #endif
                
                if((int)DHT11.humidity != 0 && (int)DHT11.temperature != 0)
                {
                    mqttBuffer[MQTT_HUMI_NO].Data = (int)DHT11.humidity;
                    mqttBuffer[MQTT_TEMP_NO].Data = (int)DHT11.temperature;
                }
                
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
  static boolean last_state = false;
  boolean current_state;
  current_state = digitalRead(PIRPIN);
  
  if(last_state != current_state)
  {
      // Timer1.detachInterrupt();
       if(current_state == HIGH){
            mqttBuffer[MQTT_MOTION_NO].Data = true;
           }
       else if(current_state == LOW){       
            mqttBuffer[MQTT_MOTION_NO].Data = false;
           } 
       last_state = current_state;
  } 
}

