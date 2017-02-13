#define MODEL 3 

//LEDS

#if MODEL == 1
const char * bonjourName = "bentov3-1";
#define LEDS_PER_STRIP 29
#define LED_STEP 1
#define NUM_STRIPS 3
#define LED_DATAPIN 15
#endif

#if MODEL == 2
const char * bonjourName = "bentov3-2";
#define LEDS_PER_STRIP 29
#define LED_STEP 1
#define NUM_STRIPS 3
#define LED_DATAPIN 15
#endif

#if MODEL == 3
const char * bonjourName = "bentov3-3";
#define LEDS_PER_STRIP 29
#define LED_STEP 1
#define NUM_STRIPS 3
#define LED_DATAPIN 15
#endif

#if MODEL == 4
const char * bonjourName = "bentov3-4";
#define LEDS_PER_STRIP 14
#define LED_STEP 1
#define NUM_STRIPS 3
#define LED_DATAPIN 13
#endif

#define TOTAL_LEDS_REAL LEDS_PER_STRIP * NUM_STRIPS


#define AUTO_ONE_STRIP 1

#if AUTO_ONE_STRIP
  #define NUM_ACTIVE_LEDS (LEDS_PER_STRIP / LED_STEP)
#else
  #define NUM_ACTIVE_LEDS (LEDS_PER_STRIP / LED_STEP)*NUM_STRIPS
#endif

