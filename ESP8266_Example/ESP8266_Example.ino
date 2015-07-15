#include <stdlib.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
#define AKKU_ADC A6

#define DEBUG 1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define SSID "cytrynowa_wro"
#define PASS "limonkowy"
#define IP "184.106.153.149" // thingspeak.com
String GET_Temp = "GET /update?key=EMMN6DL88IZYMMU4&field1=";
String GET_Akku = "GET /update?key=EMMN6DL88IZYMMU4&field3=";
SoftwareSerial monitor(10, 11); // RX, TX

void setup()
{
  Serial.begin(115200);
  sensors.begin();
  Serial.println("AT");
  delay(5000);
  if(Serial.find("OK")){
    connectWiFi();
  }
}

void loop(){
  float tempC1;
  char buffer[10];
  float akkuVoltage;
  
  akkuVoltage = 5.0*analogRead(AKKU_ADC)/1024.0;
  #ifdef DEBUG
  Serial.println("Akku");
  Serial.println(analogRead(AKKU_ADC));
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

