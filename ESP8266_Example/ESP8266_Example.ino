#include <stdlib.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>


//#define DEBUG 1

// Pinout config
#define SerialTransmit      12   //RS485 управляющий контакт на arduino pin 10
#define SerialReceive       11
#define RS485rxPin          9
#define RS485txPin          10
#define ONE_WIRE_BUS        2
#define AKKU_ADC            A6


#define RS485TransmitOn     HIGH
#define RS485TransmitOff    LOW  
#define RS485ReceiveOn      LOW
#define RS485ReceiveOff     HIGH  

#define SSID "cytrynowa_wro"
#define PASS "limonkowy"
#define IP "184.106.153.149" // thingspeak.com

struct SReadyToSendMap
{
  int anemometer  :1;
  int akku        :1;
  int temperature :1;
  int             :13;
  
    SReadyToSendMap()
    {
      memset(this, 0, sizeof(SReadyToSendMap));
    }
    
    bool operator != (SReadyToSendMap const & other) const
    {
        return *((u16*)(this)) != *((u16*)(&other));
    }
}
typedef SReadyToSendMap SReadyToSendMap;


OneWire           oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial    mySerial(RS485rxPin, RS485txPin); // RX, TX

String GET_cmd  = "GET /update?key=EMMN6DL88IZYMMU4&"
String GET_Temp = "field1=";
String GET_Wind = "field3=";
String GET_Akku = "field8=";

char buffer[10];
int windRS485Buffer[10];
int windRS485BufferIt = 0;
float windBuffer[16];
int windBufferCounter = 0;

static float tempC1      = 0;
static float akkuVoltage = 0;
static float windSpeed   = 0;

static bool doAnemometer  = true;
static bool doAkkuVoltage = true;
static bool doTemperature = true;

SReadyToSendMap readyToSendMap;

// all period in milisecond
const long anemometerPeriod  = 1000;
const long akkuVoltagePeriod = 60000;
const long temperaturePeriod = 60000;

//----------Setup----------------------------------------------
void setup()
{
  Serial.begin(115200);
  mySerial.begin(9600);
  pinMode(SerialTransmit, OUTPUT);  
  pinMode(SerialReceive, OUTPUT);
  pinMode(RS485rxPin, INPUT);  
  pinMode(RS485txPin, OUTPUT); 
  digitalWrite(SerialTransmit, RS485TransmitOff); 
  digitalWrite(SerialReceive, RS485ReceiveOff); 
  
  sensors.begin();
  Serial.println("AT");
  delay(5000);
  if(Serial.find("OK")){
    connectWiFi();
  }
}

//----------Main-----------------------------------------------
void loop(){
  doTheJob();
  
  if (doAkkuVoltage)
  {
    akkuVoltage = 5.0*analogRead(AKKU_ADC)/1024.0;
    #ifdef DEBUG
    Serial.println("Akku");
    Serial.println(analogRead(AKKU_ADC));
    Serial.println(akkuVoltage);
    #endif
    readyToSendMap.akku = true;
    doAkkuVoltage = false;
  }

  if (doTemperature)
  {
    sensors.requestTemperatures();
    tempC1 = sensors.getTempCByIndex(0);
    #ifdef DEBUG
    Serial.println("Temp1");
    Serial.println(tempC1);
    #endif
    readyToSendMap.temperature = true;
    doTemperature = false;
  }
  
  //Anemometr section
  if (doAnemometer)
  {
    sendAnemometerRequest();
    doAnemometer = false;
  }
  else
    getWindValue(windSpeed);

  //send data to thingspeak
  SReadyToSendMap zero;
  if (readyToSendMap != zero)
    sendThigsSpeakMsg(readyToSendMap);
}


//-------------------------------------------------------------------------------
void sendThigsSpeakMsg(SReadyToSendMap &readyToSendMap)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return;
  }
  cmd = GET_cmd;
  bool comma = false;
  if (readyToSendMap.anemometer)
  {
    cmd += GET_Wind;
    cmd += windSpeed;
    readyToSendMap.anemometer = 0;
    comma = true;
  }
  if (readyToSendMap.akku)
  {
    if (comma)
    {
      cmd += ",";
    }
    cmd += GET_Akku;
    cmd += akkuVoltage;
    readyToSendMap.akku = 0;
    comma = true;
  }
  if (readyToSendMap.temperature)
  {
    if (comma)
    {
      cmd += ",";
    }
    cmd += GET_Temp;
    cmd += tempC1;
    readyToSendMap.temperature = 0;
  }
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    Serial.print(cmd);
  }else{
    Serial.println("AT+CIPCLOSE");
  }
}

boolean connectWiFi(){
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(5000);
  
  if(Serial.find("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void sendAnemometerRequest()
{
    digitalWrite(SerialTransmit, RS485TransmitOn); 
    digitalWrite(SerialReceive, RS485ReceiveOff); 
    mySerial.write((uint8_t)0x02);
    mySerial.write((uint8_t)0x03);
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)0x00); 
    mySerial.write((uint8_t)0x00);
    mySerial.write((uint8_t)0x01);
    mySerial.write((uint8_t)0x84);
    mySerial.write((uint8_t)0x39);
    windBufferIt = 0;
}

void getWindValue(float &windSpeed)
{
    digitalWrite(SerialTransmit, RS485TransmitOff); 
    digitalWrite(SerialReceive, RS485ReceiveOn); 
    if(mySerial.available())
    {
      while( mySerial.available()) 
      {
        windRS485Buffer[windRS485BufferIt] = mySerial.read();
        #ifdef DEBUF
        Serial.print("windBuffer = ");
        Serial.println(windRS485Buffer[windRS485BufferIt]);
        #endif
        ++windRS485BufferIt;
      }
      if (windBufferIt >= 7)
      {
        windBuffer[windBufferCounter] = (float)(windRS485Buffer[3] * 0xFF + windRS485Buffer[4])/10;
        windBufferCounter = (++windBufferCounter)%16;
        
        float sum = 0;
        for (int i=0; i<16; ++i)
        {
          sum += windBuffer[i];
        }
        windSpeed = sum/16;
        
        readyToSendMap.anemometer = true;
        #ifdef DEBUG
        Serial.print("Wind speed = ");
        Serial.println(windSpeed);
        #endif
      }
    }
}

void doTheJob()
{
  static long lastMillisAkku  = 0;
  static long lastMillisAnemo = 0;
  static long lastMillisTemp  = 0;
  long currentMillis          = millis();
  
  if (currentMillis - lastMillisAkku > akkuVoltagePeriod)
  {
    lastMillisAkku = currentMillis;
    doAkkuVoltage = true;
  } 
  
  if (currentMillis - lastMillisAnemo > anemometerPeriod)
  {
    lastMillisAnemo = currentMillis;
    doAnemometer = true;
  } 
  
    if (currentMillis - lastMillisTemp > temperaturePeriod)
  {
    lastMillisTemp = currentMillis;
    doTemperature = true;
  } 
}

