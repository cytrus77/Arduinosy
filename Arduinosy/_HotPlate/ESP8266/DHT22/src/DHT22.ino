/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"

#define DHT1PIN 2     // what digital pin the DHT22 is conected to
#define DHT1TYPE DHT22   // there are multiple kinds of DHT sensors

#define DHT2PIN 14     // what digital pin the DHT22 is conected to
#define DHT2TYPE DHT11   // there are multiple kinds of DHT sensors

#define ONE_WIRE_BUS 4  // DS18B20 pin
const int buzzPin = 5;

DHT dht11(DHT2PIN, DHT2TYPE);
DHT dht22(DHT1PIN, DHT1TYPE);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

bool buzzerSwitch = false;
const unsigned int smokeAlarmLevel = 20;

void ds18b20Measure() {
  float temp;
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    Serial.print("DS18B20 Temperature: ");
    Serial.println(temp);
  } while (temp == 85.0 || temp == (-127.0));
}

void mq2Measure() {
  int gas = analogRead(A0) * 100 / 1024;

  if (gas >= smokeAlarmLevel) buzzerSwitch = true;
  else buzzerSwitch = false;

  Serial.print("MQ-2 Gas: ");
  Serial.println(gas);
}

void buzzer()
{
  if (buzzerSwitch)
  {
    const unsigned int freq1 = 3000;
    const unsigned int freq2 = 4000;
    unsigned int freq = freq == freq1 ? freq2 : freq1;
    tone(buzzPin, freq);
  }
  else
  {
    noTone(buzzPin);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");
}

int timeSinceLastRead = 0;
void loop() {

  // Report every 2 seconds.
  if(timeSinceLastRead > 5000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht11.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht11.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht11.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht11.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht11.computeHeatIndex(t, h, false);

    Serial.print("DHT11 Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");
  }

  // Report every 2 seconds.
  if(timeSinceLastRead > 5000) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht22.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht22.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht22.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      timeSinceLastRead = 0;
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht22.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht22.computeHeatIndex(t, h, false);

    Serial.print("DHT22 Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print(" *C ");
    Serial.print(hif);
    Serial.println(" *F");

    timeSinceLastRead = 0;

    ds18b20Measure();
    mq2Measure();
  }
  delay(1);
  timeSinceLastRead++;

  buzzer();
}
