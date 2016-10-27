// #include <Arduino.h>
//
// /*
//  * Blink
//  * Turns on the onboard LED on for one second, then off for one second, repeatedly.
//  * This uses delay() to pause between LED toggles.
//  */
//
// void setup() {
//   pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output
// }
//
// void loop() {
//   digitalWrite(BUILTIN_LED, HIGH);  // turn on LED with voltage HIGH
//   delay(1000);                      // wait one second
//   digitalWrite(BUILTIN_LED, LOW);   // turn off LED with voltage LOW
//   delay(1000);                      // wait one second
// }


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "cytrynowa_wro"
#define wifi_password "limonkowy"

#define mqtt_server "192.168.0.142"
#define mqtt_user "admin_ctr"
#define mqtt_password "zuzaMeduza"

#define MQTT_UPTIME        9003
#define MQTT_DIMMER        9301
#define MQTT_ANALOG        9302
#define MQTT_SMOKE         9303

#define DIMMER_PIN         D6
#define ANALOG_PIN         A0
#define SMOKE_PIN          D7


void callback(char* topic, byte* payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, callback, espClient);
//PubSubClient client(espClient);
long lastMsg = 0;

bool checkSmoke()
{
  static int lastValue = 0;
  static int counter = 0;
  static long lastMillis = 0;
  int value = digitalRead(SMOKE_PIN);
  if (lastValue != value)
  {
    if (counter == 0)
    {
      lastMillis = millis();
    }

    counter++;
    lastValue = value;

    Serial.print("Smoke value:");
    Serial.println(value);
    Serial.print("Smoke counter:");
    Serial.println(counter);

    long now = millis();

    if (counter > 400 && now - lastMillis < 2000)
    {
      char topicChar[6];
      char dataChar[4];
      itoa(MQTT_SMOKE, topicChar, 10);
      itoa(value, dataChar, 10);
      client.publish(topicChar, dataChar, true);

      Serial.print("now:");
      Serial.println(now);
      Serial.print("lastMillis:");
      Serial.println(lastMillis);
      Serial.print("Sending smoke alarm: XXXXXXXXXXXXXXXXXXXXXXXX");
      Serial.println(value);

      counter = 0;
      lastMillis = now;
    }

    if (counter && millis() - lastMillis > 20000)
    {
      counter = 0;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  pinMode(ANALOG_PIN, INPUT_PULLUP);
  pinMode(SMOKE_PIN, INPUT_PULLUP);
//  client.setServer(mqtt_server, 1883);
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      char topicChar[6];
      itoa(MQTT_DIMMER, topicChar, 10);
      client.subscribe(topicChar);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  int data = 0;
  int topic_int = 0;

  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  Serial.println();

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = (char) *p;
  topic_int = atoi(topic);
  int data_int  = atoi((char *) p);

  analogWrite(DIMMER_PIN, data_int * data_int / 10);

  // Free the memory
  free(p);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  checkSmoke();

  long now = millis();
  if (now - lastMsg > 1000)
  {
    lastMsg = now;
    int uptimeData = now/1000;
    Serial.print("New uptime:");
    Serial.println(uptimeData);

    char topicChar[6];
    char dataChar[8];
    itoa(MQTT_UPTIME, topicChar, 10);
    itoa(uptimeData, dataChar, 10);
    client.publish(topicChar, dataChar, true);

    uptimeData = analogRead(ANALOG_PIN);
    Serial.print("Analog value:");
    Serial.println(uptimeData);
    itoa(MQTT_ANALOG, topicChar, 10);
    itoa(uptimeData, dataChar, 10);
    client.publish(topicChar, dataChar, true);
  }
}
