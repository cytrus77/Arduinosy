#include <PubSubClient.h>
#include <ESP8266WiFi.h>
 
String orgId = "[org-id]";
String deviceType = "[device-type]";
String deviceId = "[device-id]";
String authToken = "[auth-token]";
 
const char* ssid = "Maker Zoo";
const char* password = "......";
 
// Replace [event-id] with any event id of your choice.
char* topic = "iot-2/evt/[event-id]/fmt/json";
String server = orgId + ".messaging.internetofthings.ibmcloud.com";
 
 
WiFiClient wifiClient;
PubSubClient client((char*)server.c_str(), 1883, callback, wifiClient);
 
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}
 
 
String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += '-';
  }
  return result;
}
 
void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Generate client name based on MAC address and last 8 bits of microsecond counter
  String clientName;
//  uint8_t mac[6];
//  WiFi.macAddress(mac);
//  clientName = "
  clientName += "d:";
  clientName += orgId + ":";
  clientName += deviceType + ":";
  clientName += deviceId;
 
  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str(), "use-token-auth",(char*) authToken.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Topic is: ");
    Serial.println(topic);
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
}
 
void loop() {
  static int counter = 0;
  
  // Bluemix requires root level 
  String payload = "{\"d\": {\"micros\":";
  payload += micros();
  payload += ",\"counter\":";
  payload += counter;
  payload += "}}";
  
  if (client.connected()){
    Serial.print("Sending payload: ");
    Serial.println(payload);
 
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    }
    else {
      Serial.println("Publish failed");
    }
  } else {
    Serial.println("Client not connected. Will reset and try again.");
    abort();
  }
  ++counter;
  delay(5000);
}

