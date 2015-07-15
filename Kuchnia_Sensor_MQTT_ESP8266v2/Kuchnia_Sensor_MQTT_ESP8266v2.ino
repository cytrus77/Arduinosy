#include <dht11.h>
#include <string.h>
#include "TimerOne.h"
#include "Kuchnia.h"
#include <avr/wdt.h>
#include <SoftwareSerial.h>
#include <espduino.h>
#include <mqtt.h>

//#define DEBUG 1
//#define LOG 1

SoftwareSerial debugPort(2, 3); // RX, TX
ESP esp(&Serial, &debugPort, 4);
MQTT mqtt(&esp);
boolean wifiConnected = false;
static unsigned int mainCounter = 0;

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

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      mqtt.connect(server, 1883, false);
      wifiConnected = true;
      //or mqtt.connect("host", 1883); /*without security ssl*/
    } else {
      wifiConnected = false;
      mqtt.disconnect();
    }
    
  }
}

void mqttConnected(void* response)
{
  debugPort.println("Connected");
  mqtt.subscribe("1010"); //or mqtt.subscribe("topic"); /*with qos = 0*/
  mqtt.subscribe("1020");
//  mqtt.subscribe("/topic/2");
//  mqtt.publish("/topic/0", "data0");

}

void mqttDisconnected(void* response)
{

}

void mqttData(void* response)
{
  int data = 0;
  int topic = 0;
  RESPONSE res(response);
  String topicStr = res.popString();
  String dataStr = res.popString();

  #ifdef DEBUG
  debugPort.print("Received: topic=");
  debugPort.println(topicStr);
  debugPort.print("data=");
  debugPort.println(dataStr);
  #endif
  
  topic = atoi(topicStr.c_str());
  data = atoi(dataStr.c_str());

  switch(topic)
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
}

void mqttPublished(void* response)
{

}

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);
  
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
  
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println("ARDUINO: setup mqtt client");
  if(!mqtt.begin("DVES_duino", "admin", "Isb_C4OGD4c3", 120, 1)) {
    debugPort.println("ARDUINO: fail to setup mqtt");
    while(1);
  }


  debugPort.println("ARDUINO: setup mqtt lwt");
  mqtt.lwt("/lwt", "offline", 0, 0); //or mqtt.lwt("/lwt", "offline");

/*setup mqtt events */
  mqtt.connectedCb.attach(&mqttConnected);
  mqtt.disconnectedCb.attach(&mqttDisconnected);
  mqtt.publishedCb.attach(&mqttPublished);
  mqtt.dataCb.attach(&mqttData);

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect(ssid,password);


  debugPort.println("ARDUINO: system started");
  
  //Smooth dimming interrupt init
  Timer1.initialize(2000);
  Timer1.attachInterrupt(TimerLoop);
  #ifdef DEBUG
  debugPort.println("Timer 1 wlaczony!!!!!!");
  #endif
  
  mqttConfig();
  // wdt_enable(WDTO_8S);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop() {  
  esp.process();
//  wdt_reset();
  PhotoPomiar();
  FloodPomiar();
  Mq2Pomiar();
  mainCounter++;
  
  if(wifiConnected && millis() > 5000) 
  {
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
               mqtt.publish(topicChar, dataChar);
               #ifdef DEBUG
               debugPort.println("Sending MQTT");
               debugPort.println(topicChar);
               debugPort.println(dataChar);
               #endif 
               lastPir = mqttBuffer[MQTT_MOTION_NO].Data;
               mainCounter = 1;
           }
    
        
        if(!(mainCounter%2500)){
            #ifdef LOG
            debugPort.print("Main counter: ");
            debugPort.println(mainCounter);
            #endif
            #ifdef DEBUG
            debugPort.println("Przerwanie Timer 1 - SendMQTT");
            debugPort.print("Przerwanie Timer 1 - Loop counter");
            debugPort.println(mainCounter);
            #endif
            SendMqtt();
        }
        
        if(!(mainCounter%10000)){
            #ifdef DEBUG
            debugPort.println("Przerwanie Timer 1 - DHT11");
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
        debugPort.println("PDALO POLACZENIE Z MQTT !!!!!!!!");
        #endif    
        #ifdef DEBUG
        debugPort.println("PDALO POLACZENIE Z MQTT !!!!!!!!");
        #endif
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////


void TimerLoop(){
  static unsigned int loopCounter = 0;
  loopCounter++;
  mainCounter++;
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
    debugPort.println("Przerwanie Timer 1 - Dimmer");
    #endif
  }
}


void SendMqtt(){
  static int counter = 0;
  counter = (counter + 1)%7;
  char dataChar[6];
  char topicChar[6];
  const char * topicCharPt = topicChar;
  itoa(mqttBuffer[counter].Data, dataChar, 10);
  itoa(mqttBuffer[counter].Topic, topicChar, 10);
  mqtt.publish(topicCharPt, dataChar);
      
  #ifdef LOG
  debugPort.println("Sending MQTT");
  debugPort.println(topicChar);
  debugPort.println(dataChar);
  #endif
}


void Mq2Pomiar(){
  mqttBuffer[MQTT_GAS_NO].Data = analogRead(MQ2PIN);
  mqttBuffer[MQTT_GAS_NO].Data = map(mqttBuffer[MQTT_GAS_NO].Data,0,1024,0,100);
  
  #ifdef DEBUG
  debugPort.print("Gaz: ");
  debugPort.println(mqttBuffer[MQTT_GAS_NO].Data);
  #endif
  
  if(mqttBuffer[MQTT_GAS_NO].Data > 25) 
  {
    digitalWrite(GASDIODAPIN, HIGH);
    #ifdef DEBUG
    debugPort.println("UWAGA - WYKRYTO GAZ LUB DYM");
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
  debugPort.print("Zalanie: ");
  debugPort.println(mqttBuffer[MQTT_FLOOD_NO].Data);
  #endif
}


void PhotoPomiar() {
  mqttBuffer[MQTT_PHOTO_NO].Data = analogRead(PHOTOPIN);
  mqttBuffer[MQTT_PHOTO_NO].Data = map(mqttBuffer[MQTT_PHOTO_NO].Data,0,1024,100,0);
  
  #ifdef DEBUG
  debugPort.print("Swiatlo: ");
  debugPort.println(mqttBuffer[MQTT_PHOTO_NO].Data);
  #endif
}


void DHT11Pomiar(){
  switch (DHT11.read(DHT11PIN))
  {
    case DHTLIB_OK:              
                #ifdef DEBUG
                debugPort.println("\n");
                debugPort.print("DHT 11 Read sensor: ");
                #endif
                
                if((int)DHT11.humidity != 0 && (int)DHT11.temperature != 0)
                {
                    mqttBuffer[MQTT_HUMI_NO].Data = (int)DHT11.humidity;
                    mqttBuffer[MQTT_TEMP_NO].Data = (int)DHT11.temperature;
                }
                
                #ifdef DEBUG
                debugPort.print("Humidity (%): ");
                debugPort.println((float)DHT11.humidity, 2);
              
                debugPort.print("Temperature (Â°C): ");
                debugPort.println((float)DHT11.temperature, 2);
                #endif

		//debugPort.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		//debugPort.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		//debugPort.println("Time out error"); 
		break;
    default: 
		//debugPort.println("Unknown error"); 
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

