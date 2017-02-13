#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#include <OSCMessage.h>
#include <OSCBoards.h>

#include "WifiManager.h"
#include "LedManager.h"

void faderMove(OSCMessage &msg, int addrOffset)
{
  
  Serial.println("Got message :");
  if(msg.isFloat(0))
  {
    float value = msg.getFloat(0);
    
    int intVal = (value*255);
    Serial.println(intVal);
    if(msg.match("/1/fader1")) ledR = intVal;
    else if(msg.match("/1/fader2")) ledG = intVal; 
    else if(msg.match("/1/fader3")) ledB = intVal;
    updateLed();
  }

  
}


void processOSC()
{
   OSCMessage msgIN;
   int size;

 
   if( (size = udp.parsePacket())>0)
   {
    Serial.print("Got packet :");
    Serial.println(size);
     while(size--)
       msgIN.fill(udp.read());
      
      if(!msgIN.hasError())
      {
         Serial.print("after fill :");
        Serial.println(msgIN.getFloat(0));
        msgIN.route("/1", faderMove);
      }
      else 
       Serial.println("> error");
   }
}

void setup() {
  Wire.begin(2,4);
  Serial.begin(115200);

  setupLed();   
 initWifi();
}

void loop() {
  processOSC();
  //delay(10);
}





