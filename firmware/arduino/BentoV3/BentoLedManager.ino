
//COLORS
#define COLOR_RED 0xFF0000
#define COLOR_ORANGE 0xFFAA00
#define COLOR_BLUE 0x0000FF
#define COLOR_GREEN 0x00FF00


//Declarations to be put in .h with all leds helpers
//declarations
void setRangeRGB(int startIndex, int endIndex,int r, int g, int b, bool blackBG = true);
void setRangeColor(int startIndex, int endIndex, unsigned int color, bool blackBG = true);
void setFullColor(unsigned int color);
void setLed(int ledIndex, int r, int g, int b);

void initLeds()
{
   strip.Begin();
   strip.Show();

  char info[256];
  sprintf(info, "Init BenTo with :\n %i real leds\n %i strips\n %i active leds per strip",TOTAL_LEDS_REAL,NUM_STRIPS,NUM_ACTIVE_LEDS);
  Serial.println(info);
  setFullColor(0x222222);
}


//LED COMMANDS
void setFullColor(unsigned int color)
{
  setRangeColor(0,NUM_ACTIVE_LEDS,color);
}

void setFullRGB(int r, int g, int b)
{
  setRangeRGB(0,NUM_ACTIVE_LEDS,r,g,b);
  
}

void setRangeColor(int startIndex, int endIndex, unsigned int color, bool blackBG)
{
  
  setRangeRGB(startIndex,endIndex,(color >> 16) & 0xFF,(color >> 8) & 0xFF,color & 0xFF, blackBG);
  
}

void setRangeRGB(int startIndex, int endIndex,int r, int g, int b, bool blackBG)
{
  if(blackBG) strip.ClearTo(0,0,0);
  
  for(int i=startIndex;i<endIndex;i++) setLed(i,r,g,b);  
  
  strip.Show();
}

//Helpers
void setLed(int ledIndex, int r, int g, int b)
{
    int realIndex = std::min(std::max(ledIndex,0),NUM_ACTIVE_LEDS)*LED_STEP;

    /*
    Serial.print("Set led ");
    Serial.print(ledIndex);
    Serial.print(" (real = ");
    Serial.print(realIndex);
    Serial.println(")");
    */
    strip.SetPixelColor(realIndex, r, g, b);
    
    #if AUTO_ONE_STRIP
    /*
    Serial.println(" > with autoOneStrip");
    */
    strip.SetPixelColor(LEDS_PER_STRIP*2-1-realIndex, r, g, b);
    strip.SetPixelColor(LEDS_PER_STRIP*2+realIndex, r, g, b);
    #endif
}
