#ifndef LEDSTRIP_MANAGER_H
#define LEDSTRIP_MANAGER_H

#include "FastLED.h"

#define NUM_LEDS 29
#define LEDS_PIN1 5
#define LEDS_PIN2 15


class LedStripManager
{
public:
  int ledBufferIndex;
  uint8_t colorBuffer[3];
  int colorBufferIndex;
  
  CRGBArray<NUM_LEDS> leds;
  float brightness;
    
  #if USE_WIFI
  WiFiUDP &udp;
  LedStripManager(WiFiUDP &udp):udp(udp)
  #else
  LedStripManager()
  #endif
  {
    brightness = 100;
    
    ledBufferIndex = 0;
    colorBufferIndex = 0;
    for(int i=0;i<3;i++) colorBuffer[i] = 0;
  }


  
  void init(){
    #if SERIAL_DEBUG
    Serial.println("LedStripManager Init.");
    #endif
    
    FastLED.addLeds<WS2812B, LEDS_PIN1, GRB>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812B, LEDS_PIN2,GRB>(leds, NUM_LEDS);
    //setBrightnessFactor(.2f); //20% of total brightness
    
    setFullColor(CRGB(50,50,50));
    delay(100);
    FastLED.clear();
    FastLED.show();
  }


  //Helper functions
  void setBrightnessFactor(float value)
  {
    brightness = value;
    FastLED.setBrightness((int)(brightness*255));
  }
  
  void setFullColor(CRGB color)
  {
    for(int i=0;i<NUM_LEDS;i++)
    {
      leds[i] = color;
    }

    FastLED.show();
  }

  void setRange(int startIndex, int endIndex, CRGB color, bool unique = false)
  {
    if(unique) FastLED.clear();
    
    if(startIndex > endIndex)
    {
      FastLED.show();
      return;
    }

    if(startIndex < 0) startIndex = 0;
    if(startIndex >= NUM_LEDS) startIndex = NUM_LEDS-1;
    if(endIndex < 0) endIndex = 0;
    if(endIndex >= NUM_LEDS) endIndex = NUM_LEDS-1;
    
    for(int i=startIndex;i <= endIndex;i++)
    {
      leds[i] = color;
    }

    FastLED.show();
  }

  void setLed(int index, CRGB color, bool unique = false)
  {
    if(unique) FastLED.clear();
    
    if(index < 0 || index >= NUM_LEDS)
    {
      FastLED.show();
      return;
    }
    
    leds[index] = color;
    
    FastLED.show();
  }
  
  void update()
  {
    #if USE_WIFI
    processWifi();
    #endif
  }

  #if USE_WIFI
  void processWifi()
  {
    if (udp.parsePacket())
    {
      while (udp.available() > 0)
      {
        byte b = udp.read();
  
        if (b == 255) processBuffer();
        else if (ledBufferIndex < NUM_LEDS)
        {
          colorBuffer[colorBufferIndex] = (uint8_t)b;
          colorBufferIndex++;
          if (colorBufferIndex == 3)
          {
            leds[ledBufferIndex] = CRGB(colorBuffer[0], colorBuffer[1], colorBuffer[2]);
  
            colorBufferIndex = 0;
            ledBufferIndex ++;
          }
        }
      }
      udp.flush();
    }
  
    delay(2);
    //udp.send("o 1.2543487 1.214657 0.312354589");
  }
  
  void processBuffer()
  {
    //Serial.println("process Buffer !");
    FastLED.show();
    ledBufferIndex = 0;
  }
  #endif
  
  #if USE_OSC
  boolean handleMessage(OSCMessage &msg)
  {
    
    if(msg.match("/strip"))
    {
      setFullColor(CRGB(msg.getFloat(0)*255,msg.getFloat(1)*255,msg.getFloat(2)*255));
      return true;
    }

    return false;
  }
  #endif
};

#endif
