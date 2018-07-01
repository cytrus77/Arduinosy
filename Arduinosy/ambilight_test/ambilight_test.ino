#include <Adafruit_NeoPixel.h>

// Salon
#define PIN 6
//#define LEDS_COUNT 274
#define LEDS_COUNT 26

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


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LEDS_COUNT, PIN, NEO_GRB + NEO_KHZ800);
EMode mode = Rainbow;

void setup()
{
  Serial.begin(9600);
  pixels.begin();
}


void rainbow()
{
  unsigned int R = 255;
  unsigned int G = 0;
  unsigned int B = 0;
  unsigned int* rgbColour[3] = {&R, &G, &B};

  bool initDone = false;
  unsigned int initCounter = 0;

  while (true)
  {
    unsigned int ledNo = 0;

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

        if (initDone) { pixels.show(); delay(50); }
        else if (initCounter == LEDS_COUNT) initDone = true;
        else ++initCounter;
      }
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
    pixels.show();
  }
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
    pixels.show();
  }
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
//      Serial.print("loopCount=");
//      Serial.print(loopCount);
//      Serial.print(" i=");
//      Serial.print(i);
//      Serial.print(" distanceFromCenter=");
//      Serial.print(distanceFromCenter);
//      Serial.print(" light=");
//      Serial.print(light);
//      Serial.print(" value=");
//      Serial.println(value);

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
    pixels.show();
  }
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

