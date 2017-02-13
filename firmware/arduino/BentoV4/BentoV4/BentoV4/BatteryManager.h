#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include "MAX17048.h"

#define BATT_CHECK_INTERVAL 10000
MAX17048 batteryMonitor;

class BatteryManager
{
public:

  float cellVoltage;
  float stateOfCharge;

  bool isCharging;
  long lastCheckTime;
  
  BatteryManager()
  {
    stateOfCharge = 0;
    cellVoltage = 0; 
    
    addBatteryCriticalLevelCallback(&BatteryManager::criticalDefaultCallback);
    addChargingStateChangedCallback(&BatteryManager::chargingStateChangedDefaultCallback);
    addBatteryLevelUpdateCallback(&BatteryManager::batteryLevelUpdateDefaultCallback);

    lastCheckTime = millis();
    isCharging = false;
  }

  void init()
  {
    batteryMonitor.reset();
    batteryMonitor.quickStart();
  }

  void update()
  {
    checkBattery();
  }

  void checkBattery()
  {
    
    if (millis() - lastCheckTime > BATT_CHECK_INTERVAL)
    {
      lastCheckTime = millis();
      
      float cellVoltage = batteryMonitor.getVCell();
      float stateOfCharge = batteryMonitor.getSoC();

      if(cellVoltage >= 4 && stateOfCharge >= 100)
      {
        setCharging(true);
      }else
      {
        setCharging(false);
      }
      
      if (cellVoltage < 3.1 || stateOfCharge < 10)
      {
        onBatteryCriticalLevel();
       }
    }
  }

  void setCharging(bool value)
  {
    if(isCharging == value) return;
    
    isCharging = value;
    onChargingStateChanged();
  }

  typedef void(*onCriticalEvent)();
  void (*onBatteryCriticalLevel) ();
  
  void addBatteryCriticalLevelCallback (onCriticalEvent func) { 
    onBatteryCriticalLevel = func;
  }

  static void criticalDefaultCallback()
  {
    //nothing
  }
  

  typedef void(*onChargingEvent)();
  void (*onChargingStateChanged) ();
  
  void addChargingStateChangedCallback (onChargingEvent func) { 
    onChargingStateChanged = func;
  }

  static void chargingStateChangedDefaultCallback()
  {
    //nothing
  }

  typedef void(*onBatteryUpdateEvent)();
  void (*onBatteryLevelUpdate) ();
  
  void addBatteryLevelUpdateCallback (onBatteryUpdateEvent func) { 
    onBatteryLevelUpdate = func;
  }

  static void batteryLevelUpdateDefaultCallback()
  {
    //nothing
  }
  
};

#endif
