#include <avr/wdt.h>
#include "TimerOne.h"
#include <Adafruit_NeoPixel.h>

const int LED1_PIN = 6;
const int LED1_COUNT = 78;

const int LED2_PIN = 5;
const int LED2_COUNT = 56;

const int PIR_PIN = 7;

bool lastState = false;
bool newState = false;
const unsigned int ledTimer = 2 * 120 /* sec */ ;
unsigned int timerCounter = 0;
Adafruit_NeoPixel pixelsUp = Adafruit_NeoPixel(LED1_COUNT, LED1_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixelsDown = Adafruit_NeoPixel(LED2_COUNT, LED2_PIN, NEO_GRB + NEO_KHZ800);


void rainbow(Adafruit_NeoPixel& pixels)
{
  Serial.println("rainbow");
  unsigned int R = 255;
  unsigned int G = 0;
  unsigned int B = 0;
  unsigned int* rgbColour[3] = {&R, &G, &B};

  bool initDone = false;
  unsigned int initCounter = 0;

  while (newState)
  {
    unsigned int ledNo = 0;
//    Serial.println("rainbow 1");

    // Choose the colours to increment and decrement.
    for (int decColour = 0; decColour < 3; decColour += 1)
    {
      if (!newState) break;
//      Serial.println("rainbow 2");

      int incColour = decColour == 2 ? 0 : decColour + 1;
      const int colorStep = 2;
      for(int i = 0; i < 255; i = colorStep > (255 - i) ? 255 : i + colorStep)
      {
        if (!newState) break;
//        Serial.println("rainbow 3");

        *rgbColour[decColour] = colorStep > *rgbColour[decColour] ? 0 : *rgbColour[decColour] - colorStep;
        *rgbColour[incColour] = colorStep > (255 - *rgbColour[incColour]) ? 255 : *rgbColour[incColour] + colorStep;
        pixels.setPixelColor(pixels.numPixels() - 1, R, G, B);

        for(int i=0; i < pixels.numPixels() - 1; i++)
        {
//          Serial.println("rainbow 4");

          if (!newState) break;
          uint32_t color = pixels.getPixelColor(i+1);
          unsigned int r = (color >> 16) & 0xFF;
          unsigned int g = (color >> 8) & 0xFF;
          unsigned int b = (color) & 0xFF;
          pixels.setPixelColor(i, r, g, b);
        }

        if (initDone) { pixels.show(); delay(50); }
        else if (initCounter == pixels.numPixels()) initDone = true;
        else ++initCounter;
      }
    }
  }
}

void randomizer(Adafruit_NeoPixel& pixels)
{
  for(int i=0; i < pixels.numPixels(); i++)
  {
    unsigned int R = random(255);
    unsigned int G = random(255);
    unsigned int B = random(255);     
    pixels.setPixelColor(i, R, G, B);
    pixels.show();
  }
}

void off(Adafruit_NeoPixel& pixels)
{
  Serial.println("off");
  for(int i=0; i < pixels.numPixels() ; i++)
  {
    const unsigned int off = 0;
    pixels.setPixelColor(i, off, off, off); 
  }
  pixels.setBrightness(off);
  pixels.show();
}

void whiteShow(Adafruit_NeoPixel& pixels, const unsigned int delayMs)
{
  Serial.println("whiteShow");
  unsigned int loopCount = 1;
  while (newState)
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

      pixels.setPixelColor(i, value, value, value > 0x7E ? 0x7E : value);

      if (i == 0 && value == 255) doneFlag = true;
    }
    pixels.show();
    if (delayMs) delay(delayMs);
    ++loopCount;

    if (doneFlag)
    {
      Serial.println("whiteSnow break");
      break;
    }
  }
}


void setup()
{
  Serial.begin(115200);
  pixelsUp.begin();
  pixelsDown.begin();
  pinMode(PIR_PIN, INPUT);

  Timer1.initialize(500000);  // microseconds
  Timer1.attachInterrupt(TimerLoop);
//  wdt_enable(WDTO_8S);
  
  Serial.println("Startup done");
}

void loop()
{
//  wdt_reset();
  
  if (lastState && !newState)
  {
    Serial.println("off begin");
    lastState = newState;
    off(pixelsUp);
    off(pixelsDown);
    Serial.println("off end");
  }
  else if (!lastState && newState)
  {
    Serial.println("on begin");
    lastState = newState;
    whiteShow(pixelsUp);
    whiteShow(pixelsDown);
    Serial.println("on end");
  }
}

void TimerLoop()
{
  if (digitalRead(PIR_PIN))
  {
    timerCounter = 0;
    newState = true;
    Serial.println("Move detected");
  }

  if (lastState && ++timerCounter > ledTimer)
  {
    newState = false;
    Serial.println("Timer expired");
  }

//  Serial.print("Old state = ");
//  Serial.print(lastState);
//  Serial.print(" New state = ");
//  Serial.println(newState);
}

