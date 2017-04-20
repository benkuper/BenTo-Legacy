#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "DeviceSettings.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "FS.h"

#define USE_BONJOUR 0
#define USE_SERVER 1

#if USE_BONJOUR
#include <ESP8266mDNS.h>
MDNSResponder mdns;
#else
#include <WiFiUDP.h>
#endif

#if USE_OSC
#include <OSCMessage.h>
#endif

#if USE_SERVER
#include "WifiSettingsServer.h";
#endif

class WifiManager
{
  public:

    String ssid = "Alice";
    String password = "rastafarai";

    const int maxTries = 50;

    WiFiUDP oscUDP;
    WiFiUDP streamingUDP;

    bool isConnected;

    bool apMode;

#if USE_SERVER
    WifiSettingsServer wServer;
#endif

    WifiManager() {

      apMode = false;
      isConnected = false;
      addCallbackConnectingUpdate(&WifiManager::connectingUpdateDefaultCallback);
      addCallbackWifiConfigSaved(&WifiManager::wifiConfigSavedDefaultCallback);
    }

    void init()
    {
#if SERIAL_DEBUG
      Serial.println("WifiManager init.");
#endif

      loadWifiConfig();

      // Connect to WiFi network
      #if SERIAL_DEBUG
      Serial.println(String("WifiManager connecting to "+ssid+" : "+password));
      #endif
      
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid.c_str(),password.c_str());

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

        if (tryIndex >= maxTries)
        {
          success = false;
          break;
        }
        onConnectingUpdate(tryIndex);
        tryIndex++;
      }

#if SERIAL_DEBUG
      if (!success) Serial.print("Failed");
      Serial.println();
#endif

      if (!success)
      {
        isConnected = false;
        setupLocalWifi();
        apMode = true;
      }


#if SERIAL_DEBUG
      if (apMode)
      {
        Serial.println("");
        Serial.print("Wifi AP created, IP address: ");
        Serial.println(WiFi.softAPIP());
      } else
      {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      }

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

/*
#if USE_BONJOUR
  #if SERIAL_DEBUG
        Serial.print("Registering mDNS...");
        Serial.print("...");
  #endif

      if (!mdns.begin("bento", apMode ? WiFi.softAPIP() : WiFi.localIP())) {
#if SERIAL_DEBUG
        Serial.println("Error setting up MDNS responder!");
#endif
      } else
      {
#if SERIAL_DEBUG
        Serial.print("OK : ");
        Serial.println("Registering Bonjour service...");
#endif

        mdns.addService("ledcontrol", "udp", 8888);
#endif
      }
      */
      isConnected = true;
      delay(500);
    }

    void setupLocalWifi()
    {
      //uint8 mac[6];
      //WiFi.softAPmacAddress(mac);
      //String macString((const char *)mac);
      String wifiString(String("BenTo v4.2 ") +  ESP.getChipId());

#if SERIAL_DEBUG
      Serial.print("Setting up AP Wifi : ");
      Serial.println(wifiString);
#endif


      WiFi.softAP(wifiString.c_str());

#if USE_SERVER
      wServer.init();
#endif //end USE_SERVER
    }

    void update()
    {
#if USE_SERVER
      if (apMode) 
      {
        wServer.update();
        if(wServer.ssid.length() > 0)
        {
          Serial.println("Wifi Manager saving config...");
          saveWifiConfig(wServer.ssid.c_str(),wServer.pass.c_str());
          
        }
      }
#endif
    }

#if USE_OSC
    boolean handleMessage(OSCMessage &msg)
    {

      if (msg.match("/wifiConfig"))
      {
        if (msg.size() >= 2)
        {
          char ssidData[32];
          char passData[32];
          msg.getString(0, ssidData, msg.getDataLength(0));
          msg.getString(1, passData, msg.getDataLength(1));

          saveWifiConfig(ssidData, passData);
        }
        return true;
      }

      return false;
    }
#endif

    void saveWifiConfig(const char * ssidData, const char * passData)
    {
      int formatted = SPIFFS.format();
      Serial.println(String("Formatted : ")+formatted);
      SPIFFS.begin();
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["ssid"] = "yo";//ssidData;
      json["pass"] = "wazza";//passData;

      File configFile = SPIFFS.open("/wificonf.json", "w");

      if (!configFile) {
#if SERIAL_DEBUG
        Serial.println("Failed to open config file for writing");
#endif

        return;
      }

      json.printTo(configFile);
#if SERIAL_DEBUG
      Serial.print("Wifi confid saved, ssid : ");
      Serial.print(ssidData);
      Serial.print(", password : ");
      Serial.println(passData);
#endif

      delay(500);
      onWifiConfigSaved();
    }

    bool loadWifiConfig()
    {
#if SERIAL_DEBUG
      Serial.println("Mounting Filesystem..");
#endif

      if (!SPIFFS.begin()) {
#if SERIAL_DEBUG
        Serial.println("Failed to mount file system");
#endif
        return false;
      }

      File configFile = SPIFFS.open("/wificonf.json", "r");
      if (!configFile) {
#if SERIAL_DEBUG
        Serial.println("Failed to open config file");
#endif
        return false;
      }

      size_t size = configFile.size();

      Serial.println(String("Config file size") + size);
      if (size > 1024) {
#if SERIAL_DEBUG
        Serial.println("Config file size is too large");
#endif
        return false;
      }

      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      // We don't use String here because ArduinoJson library requires the input
      // buffer to be mutable. If you don't use ArduinoJson, you may as well
      // use configFile.readString instead.
      configFile.readBytes(buf.get(), size);

      Serial.println(String("Config file : ")+buf.get());

      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      
      if (!json.success()) {
#if SERIAL_DEBUG
        Serial.println("Failed to parse config file");
#endif
        return false;
      }

      ssid = String((const char *)json["ssid"]);
      password = String((const char *)json["pass"]);

      // Real world application would store these values in some variables for
      // later use.

#if SERIAL_DEBUG
      Serial.print("Wifi config loaded, ssid : ");
      Serial.print(ssid);
      Serial.print(", password : ");
      Serial.println(password);
#endif
      return true;
    }


    //EVENTS

    typedef void(*onConnectingUpdateEvent)(int);
    void (*onConnectingUpdate) (int);

    typedef void(*onWifiConfigEvent)();
    void (*onWifiConfigSaved) ();

    void addCallbackConnectingUpdate (onConnectingUpdateEvent func) {
      onConnectingUpdate = func;
    }

    void addCallbackWifiConfigSaved (onWifiConfigEvent func) {
      onWifiConfigSaved = func;
    }

    static void connectingUpdateDefaultCallback(int curTry)
    {
      //nothing
    }

    static void wifiConfigSavedDefaultCallback()
    {
      //nothing
    }
};

#endif


