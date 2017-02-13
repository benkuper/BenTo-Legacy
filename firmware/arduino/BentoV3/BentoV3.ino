#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NeoPixelBus.h>
#include <ESP8266mDNS.h>

#include "BentoLedSettings.h"
#include "BentoWifiSettings.h"

int ledBufferIndex = 0;
uint8_t colorBuffer[3];
int colorBufferIndex = 0;

WiFiUDP udp;
NeoPixelBus strip = NeoPixelBus(TOTAL_LEDS_REAL, LED_DATAPIN);


#define USE_POWER 0

//predefinition of functions (Arduino 1.6.6)
void initLeds();
void initWifi();
void initPower();
void processWifi();
void processPower();
void sendPower();
void processSerial();

void setup(void)
{

  Serial.begin(115200);
  
  initLeds();
  initWifi();


  #if USE_POWER
  initPower();
  #endif
  
  
  strip.ClearTo(0,0,0);
  strip.Show();

  delay(500);
}

void loop(void)
{
  processWifi();
  
  #if USE_POWER
  processPower();
  #endif
  
  processSerial();
  
  delay(5);
}


