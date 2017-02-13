#include <Arduino.h>

#define SERIAL_DEBUG 1
#include <Wire.h>


#define USE_WIFI 1

#if USE_WIFI
#define USE_OSC 1
#endif

#define USE_TOUCH 0
#define USE_IMU 1
#define USE_LED 1
#define USE_LEDSTRIP 1
#define USE_BATTERY 0
#define USE_FLASH 0

#include "DeviceSettings.h"
DeviceSettings settings;

#if USE_WIFI
#include "WifiManager.h"
WifiManager wifiManager;
#endif

#if USE_OSC
#include "OSCManager.h"
OSCManager oscManager(wifiManager.oscUDP);
#endif

#if USE_IMU
#include "IMUManager.h"
IMUManager imuManager;
#endif

#if USE_LED
#include "LedManager.h"
LedManager ledManager;
#endif

#if USE_LEDSTRIP
#include "LedStripManager.h"
  #if USE_WIFI
  LedStripManager stripManager(wifiManager.streamingUDP);
  #else
  LedStripManager stripManager;
  #endif
#endif

#if USE_TOUCH
#include "TouchManager.h"
TouchManager touchManager;
#endif

#if USE_BATTERY
#include "BatteryManager.h"
BatteryManager batteryManager;
#endif

#if USE_FLASH
#include "FlashManager.h"
FlashManager flashManager;
#endif

long orientationSendRateMS = 20;
long lastOrientationSendTime = 0;

//Callbacks

#if USE_WIFI
void wifiConnectingUpdate(int curTry)
{
    #if USE_LEDSTRIP
    stripManager.setRange(0,curTry%NUM_LEDS,CRGB(30,50,255),true);
    #endif

}
#endif

#if USE_TOUCH
void touchUpdate(int touchID, bool touched)
{
  #if SERIAL_DEBUG
  Serial.print("t\t");
  Serial.print(touchID);
  Serial.print("\t");
  Serial.println(touched);
  #endif

  #if USE_OSC
  OSCMessage msg("/touch");
  msg.add(DeviceSettings::deviceID);
  msg.add(touchID);
  msg.add(touched);
  oscManager.sendMessage(msg);
  #endif
}
#endif

#if USE_IMU
void orientationUpdate(float yaw, float pitch, float roll)
{
  if(millis() - lastOrientationSendTime > orientationSendRateMS)
  {
    #if SERIAL_DEBUG
    Serial.print("o\t");
    Serial.print(yaw);
    Serial.print('\t');
    Serial.print(pitch);
    Serial.print('\t');
    Serial.println(roll);
    #endif
  
    #if USE_OSC
    OSCMessage msg("/orientation");
    msg.add(DeviceSettings::deviceID);
    msg.add(yaw);
    msg.add(pitch);
    msg.add(roll);
    oscManager.sendMessage(msg);
    #endif

    lastOrientationSendTime = millis();
  }
}
#endif


#if USE_OSC
void messageReceived(OSCMessage &msg)
{
  #if SERIAL_DEBUG
  Serial.println("OSC Message received.");
  #endif

  if(msg.match("/reset"))
  {
    resetESP(false);
  }else if(msg.match("/bootloader"))
  {
    resetESP(true);
  }else
  {
    #if USE_TOUCH
    if(touchManager.handleMessage(msg)) return; 
    #endif
    
    #if USE_LED
    if(ledManager.handleMessage(msg)) return; 
    #endif
  
    #if USE_LEDSTRIP
    if(stripManager.handleMessage(msg)) return;
    #endif

    #if SERIAL_DEBUG
    Serial.println("...message not handled");
    #endif
  }
    
}
#endif


#if USE_BATTERY
void batteryCriticalLevel()
{
  #if DEBUG_SERIAL
  Serial.println("CRITICAL LEVEL, DEEP SLEEP !");
  #endif
  
  delay(100);
  ESP.deepSleep(30 * 1000000); //will not be able to wake up (RST not connected to pin 14)
}

void batteryChargingStateChanged()
{
  #if SERIAL_DEBUG
  Serial.print("Battery chargin state changed : ");
  Serial.print(batteryManager.isCharging);
  Serial.println("");
  #endif
}

void batteryLevelUpdate()
{
  #if SERIAL_DEBUG
  Serial.print("Battery level update : ");
  Serial.print(batteryManager.stateOfCharge);
  Serial.println("%");
  #endif
}
#endif


void resetESP(bool toBootloader)
{
  if(toBootloader)
  {
    pinMode(0,OUTPUT);
    digitalWrite(0,LOW);
  }
  
  ESP.reset();
}

//Setup & loop
void setup() {

  
  delay(500);  

  Wire.begin(2,4);
  
  Serial.begin(115200);
  #if SERIAL_DEBUG
  
  #endif


  settings.init();
  
  #if USE_LED
  ledManager.init();
  delay(200);
  #endif
  
   delay(300);

  #if SERIAL_DEBUG
  Serial.println("BentoV4 Initialization.");
  Serial.println(ESP.getResetReason());
  Serial.println("Will init all modules");
  #endif
  
  #if USE_LEDSTRIP
  stripManager.init();
  #endif

   #if USE_TOUCH
  touchManager.init();
  touchManager.addCallbackTouchUpdate(&touchUpdate);
  #endif

  #if USE_IMU
  imuManager.init();
  imuManager.addCallbackOrientationUpdate(&orientationUpdate);
  #endif

  

  //Wifi & OSC
  #if USE_WIFI
    wifiManager.addCallbackConnectingUpdate(&wifiConnectingUpdate);
    wifiManager.init();
    
    #if USE_LEDSTRIP
    stripManager.setFullColor(wifiManager.isConnected?CRGB::Green:CRGB::Red);
    delay(300);
    stripManager.setFullColor(CRGB::Black);
    #endif
   
    #if USE_OSC
    oscManager.init();
    oscManager.addCallbackMessageReceived(&messageReceived);
    #endif

  #endif

  #if USE_BATTERY
  batteryManager.init();
  batteryManager.addBatteryCriticalLevelCallback(&batteryCriticalLevel);
  batteryManager.addChargingStateChangedCallback(&batteryChargingStateChanged);
  batteryManager.addBatteryLevelUpdateCallback(&batteryLevelUpdate);
  #endif

  #if USE_LEDSTRIP 
  stripManager.setFullColor(CRGB::Black);
  #endif

  #if SERIAL_DEBUG
  Serial.println("Bento is READY :)");
  #endif
}


void loop() {
  // put your main code here, to run repeatedly:
  
  #if USE_WIFI
    wifiManager.update();
  
    #if USE_OSC
    oscManager.update();
    #endif
      
  #endif

  
  #if USE_TOUCH
  touchManager.update();
  #endif

  #if USE_IMU
  imuManager.update();
  #endif

  #if USE_LEDSTRIP
  stripManager.update();
  #endif

  #if USE_BATTERY
  batteryManager.update();
  #endif


  processSerial();
}

void processSerial()
{
  if(Serial.available()  == 0) return;

  while(Serial.available() > 0)
  {
    char c = Serial.read();
    switch(c)
    {
      case 'r':
      resetESP(false);
      break;
      
      case 'b':
      resetESP(true);
      break;

    }
  }
}

