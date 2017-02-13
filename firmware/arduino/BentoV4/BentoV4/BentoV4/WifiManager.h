#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define USE_BONJOUR 0

#if USE_BONJOUR
#include <ESP8266mDNS.h>
#else
#include <WiFiUDP.h>
#endif


class WifiManager
{
public:

  const char* ssid = "BenTo BK";
  const char* password = "konamicode";
  
  const int maxTries = 50;
  
  WiFiUDP oscUDP;
  WiFiUDP streamingUDP;
  
  bool isConnected;
  
  WifiManager(){
    
    isConnected = false; 
    addCallbackConnectingUpdate(&WifiManager::connectingUpdateDefaultCallback);
  }
  
  void init()
  { 
    #if SERIAL_DEBUG
    Serial.println("WifiManager init.");
    
    #endif
    
    // Connect to WiFi network
    WiFi.begin(ssid, password);
    
    int tryIndex = 0;
    bool success = true;

    #if SERIAL_DEBUG
    Serial.print("Connecting : ");
    #endif
    
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      
      #if SERIAL_DEBUG
      Serial.print("*");
      #endif
      
      if(tryIndex >= maxTries) 
      {
        success = false;
        break;
      }
      onConnectingUpdate(tryIndex);
      tryIndex++;
    }

    #if SERIAL_DEBUG
      if(!success) Serial.print("Failed");
      Serial.println();
    #endif
      
    if(!success)
    {
      isConnected = false;
      return;
    }
    
    
    #if SERIAL_DEBUG
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    #endif
    
    
    delay(500);

    #if SERIAL_DEBUG
    Serial.print("Setting up UDP...");
    #endif
    
     oscUDP.begin(9000);
     streamingUDP.begin(8888);

     #if SERIAL_DEBUG
     Serial.println("OK");
    #endif

    #if USE_BONJOUR
      #if SERIAL_DEBUG
        Serial.print("Registering mDNS with name ");
        Serial.print(bonjourName);
        Serial.print("...");
      #endif
      
      if (!MDNS.begin(bonjourName)) {
        #if SERIAL_DEBUG
        Serial.println("Error setting up MDNS responder!");
        #endif
        while(1) { 
          delay(1000);
        }
      }
  
      #if SERIAL_DEBUG
      Serial.print("OK : ");
      Serial.println("Registering Bonjour service...");
      #endif
  
      
      MDNS.addService("ledcontrol", "udp", 8888);
    #endif

    #if SERIAL_DEBUG
    Serial.println("OK");
    #endif
    
    isConnected = true;
    
    delay(500);
  }

  void update()
  {
    
  }



  typedef void(*onEvent)(int);
  void (*onConnectingUpdate) (int);
  
  void addCallbackConnectingUpdate (onEvent func) { 
    onConnectingUpdate = func;
  }

  static void connectingUpdateDefaultCallback(int curTry)
  {
    //nothing
  }
};

#endif


