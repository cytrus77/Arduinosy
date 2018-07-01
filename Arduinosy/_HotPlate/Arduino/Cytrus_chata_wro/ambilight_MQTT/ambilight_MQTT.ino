#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define DEBUG 1

enum EMode : unsigned int
{
  Off,
  White,
  Yellow,
  Rainbow,
  Random,
  WhiteShow,
  Polska
};


// Ethernet config
const byte mac[]    = {  0xDA, 0xED, 0xBA, 0xFE, 0xFE, 0xEF };
const byte server[] = { 192, 168, 38, 11};
const byte ip[]     = { 192, 168, 38, 240 };
const String deviceName = "salonLed";
const unsigned int mqttPort = 1883;

void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, mqttPort, callback, ethClient);

// MQTT
const String brightnessTopic = deviceName + "/brightness";
const String modeTopic = deviceName + "/mode";
unsigned int brightness = 0;
EMode mode = Off;

// WS2812 LED config
#define PIN 6           // Salon
#define LEDS_COUNT 274  // Salon

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS_COUNT, PIN, NEO_GRB + NEO_KHZ800);

////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  Ethernet.begin(mac, ip);
  delay(500);
  if (client.connect(deviceName.c_str()))
  {
    delay(200);
    client.subscribe(brightnessTopic.c_str());
    client.subscribe(modeTopic.c_str());
  }
  
  pixels.begin();

  #ifdef DEBUG
  Serial.println("Setup done");
  #endif
}

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  String topicStr = topic;
  String data;
  
  #ifdef DEBUG
  Serial.print(topic);
  Serial.print(" => ");
  Serial.write(payload, length);
  #endif
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length+1);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  p[length] = 0;
  data = (char *)p;


  if (topicStr.indexOf(brightnessTopic) >= 0)
  {
    brightness = atoi(data.c_str());
  }
  else if (topicStr.indexOf(modeTopic) >= 0)
  {
    mode = atoi(data.c_str());
  }
  else
  {
    #ifdef DEBUG
    Serial.println("Unsupported mqtt topic");
    #endif
  }
  
  // Free the memory
  free(p);
}

void rainbow()
{
  static unsigned int R = 255;
  static unsigned int G = 0;
  static unsigned int B = 0;
  static unsigned int* rgbColour[3] = {&R, &G, &B};

  static bool initDone = false;
  static unsigned int initCounter = 0;

    static unsigned int ledNo = 0;

    // Choose the colours to increment and decrement.
    for (int decColour = 0; decColour < 3; decColour += 1)
    {
      int incColour = decColour == 2 ? 0 : decColour + 1;
      const int colorStep = 2;
      for(int i = 0; i < 255; i = colorStep > (255 - i) ? 255 : i + colorStep)
      {
        *rgbColour[decColour] = colorStep > *rgbColour[decColour] ? 0 : *rgbColour[decColour] - colorStep;
        *rgbColour[incColour] = colorStep > (255 - *rgbColour[incColour]) ? 255 : *rgbColour[incColour] + colorStep;
        pixels.setPixelColor(LEDS_COUNT - 1, R, G, B);

        for(int i=0; i < LEDS_COUNT - 1; i++)
        {
          uint32_t color = pixels.getPixelColor(i+1);
          unsigned int r = (color >> 16) & 0xFF;
          unsigned int g = (color >> 8) & 0xFF;
          unsigned int b = (color) & 0xFF;
          pixels.setPixelColor(i, r, g, b);
        }

        if (initDone) { pixels.show(); /*delay(50);*/ }
        else if (initCounter == LEDS_COUNT) initDone = true;
        else ++initCounter;
      }
    }
}

void randomizer()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    unsigned int R = random(255);
    unsigned int G = random(255);
    unsigned int B = random(255);     
    pixels.setPixelColor(i, R, G, B);
  }
  pixels.show();
}

void whiteSinus()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    const float period = 10;
    unsigned int white = (sin(i/period))*256;
    white = white > 255 ? 0 : white;
    Serial.println(white);
    pixels.setPixelColor(i, white, white, white); 
  }
  pixels.show();
}

void whiteShow(Adafruit_NeoPixel& pixels, const unsigned int delayMs)
{
  Serial.println("whiteShow");
  unsigned int loopCount = 1;
  while (true)
  {
    bool doneFlag = false;
    
    for(int i=0; i < pixels.numPixels(); ++i)
    {
      int distanceFromCenter = pixels.numPixels()/2 - i;
      distanceFromCenter = distanceFromCenter > 0 ? distanceFromCenter : -distanceFromCenter;
      const int light = loopCount - (distanceFromCenter * 10);
      unsigned int value = light > 0 ? light : 0;
      value = value > 255 ? 255 : value;
      pixels.setPixelColor(i, value, value, value > 0x40 ? 0x40 : value);

      if (i == 0 && value == 255) doneFlag = true;
    }
    pixels.show();
    delay(delayMs);
    ++loopCount;

    if (doneFlag)
    {
      Serial.println("whiteSnow break");
      break;
    }
  }
}

void yellow()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    const unsigned int R = 200;
    const unsigned int G = 200;
    const unsigned int B = 50;
    pixels.setPixelColor(i, R, G, B); 
  }
  pixels.setBrightness(150);
  pixels.show();
}

void white()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    const unsigned int white = 255;
    pixels.setPixelColor(i, white, white, white); 
  }
  pixels.setBrightness(200);
  pixels.show();
}

void polska()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    unsigned int R = 255;
    unsigned int G = 255;
    unsigned int B = 255;  
      
    if (i > LEDS_COUNT/2)
    {
      G = 0;
      B = 0;  
    }
   
    pixels.setPixelColor(i, R, G, B);
  }
  pixels.show();
}

void off()
{
  for(int i=0; i < LEDS_COUNT; i++)
  {
    const unsigned int off = 0;
    pixels.setPixelColor(i, off, off, off); 
  }
  pixels.setBrightness(off);
  pixels.show();
}


void loop()
{
  client.loop();

  if(!client.connected())
  {
    #ifdef DEBUG
    Serial.println("Disconnected from MQTT server");
    #endif
    
    if(client.connect("KuchniaClient"))
    {            
      #ifdef DEBUG
      Serial.println("Connected to MQTT server");
      #endif
      delay(500);
      client.subscribe(brightnessTopic.c_str());
      client.subscribe(modeTopic.c_str());
    }
  }

  switch(mode)
  {
    case Off:
      off();
      break;
      
    case White:
      white();
      break;

    case Yellow:
      yellow();
      break;

    case Rainbow:
      rainbow();
      break;

    case Random:
      randomizer();
      break;

    case Polska:
      polska();
      break;

    case WhiteShow:
      whiteShow(pixels, 5);
      break;
      
    default:
      break;
  }
}

