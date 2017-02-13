// Test the LED strips while checking the battery SoC and shut everything down if the battery is low.

#include "FastLED.h"
#include "MAX17048.h"
#include "Wire.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
// 

// How many leds are in the strip?
#define NUM_LEDS 30
#define BRIGHTNESS 50

// Data pin that led data will be written out over
#define DATA_PIN_1 5
#define DATA_PIN_2 15

// Battery check period (ms)
#define BATT_CHECK_INTERVAL 1000

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGBArray<NUM_LEDS> leds;

MAX17048 batteryMonitor;

// This function sets up the ledsand tells the controller about them
void setup() {
    Wire.begin(2,4); 
    Serial.begin(115200);
    batteryMonitor.reset();
    batteryMonitor.quickStart();
    
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
   	delay(500);

    FastLED.addLeds<WS2812, DATA_PIN_1, GRB>(leds, NUM_LEDS);
    FastLED.addLeds<WS2812, DATA_PIN_2, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(  BRIGHTNESS );
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop() {
  checkBattery();

  //Rainbow
/*  static int start = 0;
  leds.fill_rainbow(start++, 256/NUM_LEDS);
  FastLED.show();
  delay(20); */

  //Stress test (white, brightness increasing)
  leds.fill_solid(CRGB::Red);
  static int brightness = 0;

  //brightness += 10;
  //if (brightness > 200)
  //brightness = 0;
  brightness = BRIGHTNESS;

  Serial.print("brightness ");
  Serial.println(brightness);

  FastLED.setBrightness(brightness);
  FastLED.show();
  delay(1000); 
  

  //RGBSetDemo
  /*static uint8_t hue;
  for(int i = 0; i < NUM_LEDS/2; i++) {   
    // fade everything out
    leds.fadeToBlackBy(40);

    // let's set an led value
    leds[i] = CHSV(hue++,255,255);

    // now, let's first 20 leds to the top 20 leds, 
    leds(NUM_LEDS/2,NUM_LEDS-1) = leds(NUM_LEDS/2 - 1 ,0);
    FastLED.delay(33);
  } */
  
   // Move a single white led 
/*   for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
   }*/
   
}

//Check periodically the battery state
void checkBattery()
{
  static unsigned long lastCheckTime = millis();

  if (millis() - lastCheckTime > BATT_CHECK_INTERVAL)
  {
    lastCheckTime = millis();
    
    float cellVoltage = batteryMonitor.getVCell();
    Serial.print("Voltage:\t");
    Serial.print(cellVoltage, 4);
    Serial.print("V");
    
    float stateOfCharge = batteryMonitor.getSoC();
    Serial.print(" SoC:\t");
    Serial.print(stateOfCharge);
    Serial.println("%");

    if (cellVoltage < 3.1 || stateOfCharge < 10)
    {
      //turn off all the LEDs and enter a sleep state (all sensors should be turned to sleep state as well)

      FastLED.clear();
      FastLED.show();

      delay(100);
      
      ESP.deepSleep(30 * 1000000); //will not be able to wake up (RST not connected to pin 14)
    }
  }

}

