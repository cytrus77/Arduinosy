#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>

#include "Defines.h"
#include "Dimmer.h"
#include "StatusLed.h"
#include "Uptime.h"
#include "pwm.h"
#include "Utils.h"
#include <DallasTemperature.h>
#include <Adafruit_INA219.h>


#define DEBUG 1

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

long lastMqttReconnectAttempt = 0;

WiFiClient espClient;
PubSubClient mqttClient(server, 1883, callback, espClient);

statusled StatusLed(STATUS_PIN, statusled::off, INT_TIMER_PERIOD);
uptime Uptime(MQTT_UPTIME, &mqttClient);
Dimmer ledDimmer(DIMMER_PIN, MQTT_DIMMER, MQTT_DIMMET_TIMEOUT, DIMMER_TIMEOUT, CYCLES_PER_SECOND);

Adafruit_INA219 ina219;

OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
int ds18b20timer = 0;

void setup_wifi(void);
void setup_gpio(void);
boolean mqttConnect(void);
void TimerLoop(void);
void sendAllSubscribers(void);

void i2c_manualWrite()
{
  byte error;
  byte address = 0x40;
  int nDevices;
  nDevices = 0;

  for (address = 1; address < 127; address++ )  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0){
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  } //for loop

  // Wire.beginTransmission(address);
  // error = Wire.endTransmission();
  //
  // if (error == 0)
  // {
  //   Serial.print("I2C device found at address 0x");
  // }
  // else if (error == 4)
  // {
  //   Serial.print("Unknow error at address 0x");
  //   if (address < 16)
  //     Serial.print("0");
  //   Serial.println(address, HEX);
  // }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("**********************************\n");
  delay(1);           // wait 1 seconds for next scan, did not find it necessary
}

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup()
{
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup()");
  #endif

  StatusLed.setMode(statusled::poweron);
  setup_gpio();
  setup_wifi();

  //Smooth dimming interrupt init
  lastMqttReconnectAttempt = 0;

  ina219.begin();
  ina219.setCalibration(0x100D);

  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(TimerLoop);
  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
  interrupts();

  Serial.println("StartedX");

  delay(200);
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  #ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  #endif
}

void setup_gpio(void)
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  ledDimmer.setValue(255);

  // enable all outputs
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, HIGH);
}
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop()
{
  if(mqttClient.connected())
  {
    mqttClient.loop();
    Uptime.getUptime();
    Uptime.sendIfChanged();

    StatusLed.setMode(statusled::online);

    if (ds18b20timer > (CYCLES_PER_SECOND * 60))
    {
      doCurrentMeasure();
      measureTempAndSend();
      ds18b20timer = 0;
    }
  }
  else
  {
     #ifdef DEBUG
     Serial.println("MQTT Disconnected");
     #endif

     StatusLed.setMode(statusled::offline);

     long now = millis();
     if (now - lastMqttReconnectAttempt > 3000)
     {
        lastMqttReconnectAttempt = now;
        // Attempt to reconnect
        if (mqttConnect())
        {
           lastMqttReconnectAttempt = 0;
        }
     }
  }
}
/////////////////////////////////////////////END MAIN LOOP/////////////////////////////////////////////////

void TimerLoop()
{
  ledDimmer.setDimmer();
  ledDimmer.processTimer();
  StatusLed.processTimer();

  if (ledDimmer.getCurrentValue() == 0)
  {
    digitalWrite(RELAY_PIN, LOW);
  }

  ++ds18b20timer;

  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
}

void measureTempAndSend()
{
  ds18b20.requestTemperatures();
  float temp = ds18b20.getTempCByIndex(0);
  sendMqtt(MQTT_TEMP, temp);
}

void sendMqtt(String topic, float value)
{
  char dataChar[8];
  ftoa(value, dataChar);
  mqttClient.publish(topic.c_str(), dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT float");
  Serial.println(topic);
  Serial.println(dataChar);
  #endif
}

void sendMqtt(String topic, int value)
{
  char dataChar[6];
  itoa(value, dataChar, 10);
  mqttClient.publish(topic.c_str(), dataChar);

  #ifdef DEBUG
  Serial.println("Sending MQTT");
  Serial.println(topic);
  Serial.println(dataChar);
  #endif
}

boolean mqttConnect()
{
  if (mqttClient.connect(mqtt_login.c_str(), mqtt_user.c_str(), mqtt_password.c_str()))
  {
    Serial.println("MQTT Connected");
    delay(200);
    sendAllSubscribers();
  }

  return mqttClient.connected();
}

void sendAllSubscribers(void)
{
    mqttClient.subscribe(ledDimmer.getMqttTopic().c_str());
    mqttClient.subscribe(ledDimmer.getTimeoutMqttTopic().c_str());
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length)
{
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;

  #ifdef DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  Serial.println();
  #endif

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = (char) *p;
  int data_int  = atoi((char *) p);

  if (String(topic) == ledDimmer.getMqttTopic())
  {
    ledDimmer.setValue(data_int);

    if (data_int == 0)
    {
      #ifdef DEBUG
      Serial.println("RELAY - LOW");
      #endif
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);

      #ifdef DEBUG
      Serial.println("RELAY - HIGH");
      #endif
    }
  }
  else if (String(topic) == ledDimmer.getTimeoutMqttTopic())
  {
    ledDimmer.setTimeout(data_int*60);
  }
  else if (String(topic) == MQTT_SUBSCRIBE)
  {
    sendAllSubscribers();
  }

  // Free the memory
  free(p);
}

void doCurrentMeasure()
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage  = ina219.getBusVoltage_V();
  current_mA  = ina219.getCurrent_mA();
  power_mW    = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  sendMqtt(MQTT_VOLTAGE, loadvoltage);
  sendMqtt(MQTT_CURRENT, current_mA);
  sendMqtt(MQTT_POWER, power_mW);

  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
}
