#include <SD.h>
#include <stdlib.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "TimerOne.h"

//#define DEBUG 1

#define SSID "cytrynowa_wro"
#define PASS "limonkowy"
#define IP "184.106.153.149" // thingspeak.com

//Serial RX - ESP8266 - PIN 0
//Serial TX - ESP8266 - PIN 1
#define DEBUG_RX_PIN 2
#define DEBUG_TX_PIN 3
#define ONE_WIRE_BUS 5
#define AKKU_ADC_PIN A6

/*
 This example shows how to read and write data to and from an SD card file  
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4  
 */

SoftwareSerial monitor(DEBUG_RX_PIN, DEBUG_TX_PIN); // RX, TX

const int chipSelect = 4;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
File myFile;

String GET_Anemo = "GET /update?key=EMMN6DL88IZYMMU4&field1=";
String GET_Akku = "GET /update?key=EMMN6DL88IZYMMU4&field2=";
String GET_Temp = "GET /update?key=EMMN6DL88IZYMMU4&field3=";

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  Serial.println("AT");
  delay(5000);
  if(Serial.find("OK")){
    connectWiFi();
  }
  
  //Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin 
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output 
  // or the SD library functions will not work. 
   pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    //Serial.println("initialization failed!");
    return;
  }
  //Serial.println("initialization done.");
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    //Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    //Serial.println("error opening test.txt");
  }
  
    //Smooth dimming interrupt init
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(TimerLoop);
}

void loop(){
  float tempC1;
  char buffer[10];
  float akkuVoltage;
  
  akkuVoltage = 5.0*analogRead(AKKU_ADC_PIN)/1024.0;
  #ifdef DEBUG
  Serial.println("Akku");
  Serial.println(analogRead(AKKU_ADC_PIN));
  Serial.println(akkuVoltage);
  #endif
  updateAkku(dtostrf(akkuVoltage, 4, 3, buffer));
  delay(30000);

  sensors.requestTemperatures();
  tempC1 = sensors.getTempCByIndex(0);
  #ifdef DEBUG
  Serial.println("Temp1");
  Serial.println(tempC1);
  #endif
  String tempCstr = dtostrf(tempC1, 4, 1, buffer);
  updateTemp(tempCstr);
  delay(30000);
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop(){

}

void updateTemp(String tenmpF){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return;
  }
  cmd = GET_Temp;
  cmd += tenmpF;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    Serial.print(cmd);
  }else{
    Serial.println("AT+CIPCLOSE");
  }
}
 
void updateAkku(String tenmpF){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    return;
  }
  cmd = GET_Akku;
  cmd += tenmpF;
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
  if(Serial.find("OK")){
    return true;
  }else{
    return false;
  }
}

