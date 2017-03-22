#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>

#include "Defines.h"
#include "Uptime.h"
#include "pwm.h"
#include "Utils.h"

#define DEBUG 1

void ftoa(float Value, char* Buffer);
void callback(char* topic, byte* payload, unsigned int length);

#define mqtt_login     "CTR_air_q"
#define mqtt_user      "admin"
#define mqtt_password  "Isb_C4OGD4c3"

#define wifi_ssid "cytrynowa_wro"
#define wifi_password "limonkowy"
byte server[] = { 192, 168, 0, 142 };

long lastMqttReconnectAttempt = 0;

WiFiClient espClient;
PubSubClient client(server, 1883, callback, espClient);
uptime Uptime(MQTT_UPTIME, &client);

void setup_wifi(void);
boolean mqttConnect(void);
void TimerLoop(void);
void sendAllSubscribers(void);


#define PIN D7
#define NUM_LED 1 // 31x21
#define NUM_DATA 5 // NUM_LED * 3 + 2
#define RECON_TIME 2000 // after x seconds idle time, send afk again.

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LED, PIN, NEO_GRB + NEO_KHZ800);

/////////////////////////////////////////////START SETUP/////////////////////////////////////////////////
void setup()
{
  #ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup()");
  #endif

  strip.begin();
  // strip.show(); // Initialize all pixels to 'off'
  strip.setPixelColor(0, strip.Color(10, 10, 0));
  strip.setPixelColor(1, strip.Color(10, 10, 0));

  setup_wifi();
  //Smooth dimming interrupt init
  lastMqttReconnectAttempt = 0;

  // noInterrupts();
  // timer0_isr_init();
  // timer0_attachInterrupt(TimerLoop);
  // timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
  // interrupts();

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

  WiFi.begin(wifi_ssid, wifi_password);

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
/////////////////////////////////////////////END SETUP/////////////////////////////////////////////////

/////////////////////////////////////////////START MAIN LOOP/////////////////////////////////////////////////
void loop()
{
  if(client.connected())
  {
    strip.setPixelColor(0, strip.Color(0, 10, 10));

    client.loop();
    Uptime.getUptime();
    Uptime.sendIfChanged();
  }
  else
  {
    strip.setPixelColor(0, strip.Color(10, 0, 10));

     #ifdef DEBUG
     Serial.println("Disconnected");
     #endif

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
  timer0_write(ESP.getCycleCount() + TIMER_PERIOD_NODEMCU);
}

void sendMqtt(int topic, int value)
{
  char topicChar[6];
  char dataChar[6];
  itoa(topic, topicChar, 10);
  itoa(value, dataChar, 10);
  client.publish(topicChar, dataChar);

  Serial.println("Sending MQTT");
  Serial.println(topicChar);
  Serial.println(dataChar);
}

boolean mqttConnect()
{
  if (client.connect(mqtt_login, mqtt_user, mqtt_password)) // Kuchnia
  {
    Serial.println("Connected");
    delay(200);
    sendAllSubscribers();
  }

  return client.connected();
}

void sendAllSubscribers(void)
{}

// Callback function
void callback(char* topic, byte* payload, unsigned int length)
{
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  int topic_int = 0;

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
  topic_int = atoi(topic);
  int data_int  = atoi((char *) p);

  if (topic_int == MQTT_SUBSCRIBE)
  {
    sendAllSubscribers();
  }

  // Free the memory
  free(p);
}
