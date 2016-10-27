#include <Arduino.h>

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
#define DIMMERPIN          BUILTIN_LED

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

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
  }
}
