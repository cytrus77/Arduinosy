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

OneWire           oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial    mySerial(RS485rxPin, RS485txPin); // RX, TX

String GET_Temp = "GET /update?key=EMMN6DL88IZYMMU4&field1=";
String GET_Wind = "GET /update?key=EMMN6DL88IZYMMU4&field3=";
String GET_Akku = "GET /update?key=EMMN6DL88IZYMMU4&field8=";

char buffer[10];
int windBuffer[10];
int windBufferIt = 0;

static float tempC1      = 0;
static float akkuVoltage = 0;
static float windSpeed   = 0;

static bool doAnemometer  = true;
static bool doAkkuVoltage = true;
static bool doTemperature = true;

// all period in milisecond
const long anemometerPeriod  = 10000;
const long akkuVoltagePeriod = 66000;
const long temperaturePeriod = 300000;

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
    sendThigsSpeakMsg(GET_Akku, dtostrf(akkuVoltage, 4, 2, buffer));
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
    sendThigsSpeakMsg(GET_Temp, dtostrf(tempC1, 4, 1, buffer));
    doTemperature = false;
  }
  
  //Anemometr section
  if (doAnemometer)
  {
    sendRequest();
    doAnemometer = false;
  }
  else
    getWindValue(windSpeed);
  
}


//-------------------------------------------------------------------------------
void sendThigsSpeakMsg(String getField, String value)
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return;
  }
  cmd = getField;
  cmd += value;
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

void sendRequest()
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
        windBuffer[windBufferIt] = mySerial.read();
        #ifdef DEBUF
        Serial.print("windBuffer = ");
        Serial.println(buffer[windBufferIt]);
        #endif
        ++windBufferIt;
      }
      if (windBufferIt >= 7)
      {
        windSpeed = (float)(windBuffer[3] * 0xFF + windBuffer[4])/10;
        #ifdef DEBUG
        Serial.print("Wind speed = ");
        Serial.println(windSpeed);
        #endif
        sendThigsSpeakMsg(GET_Wind, dtostrf(windSpeed, 4, 1, buffer));
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

