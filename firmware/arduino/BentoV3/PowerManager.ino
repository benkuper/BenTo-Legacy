bool criticalLevel;
float currentPower = 0;

long timeSinceLastPowerCheck = 0;
long powerCheckTime = 1000; //ms

bool charging = false;

extern "C" {
#include "user_interface.h"
uint16 readvdd33(void);
}

void initPower()
{
  timeSinceLastPowerCheck = millis()+1000;
}

void checkPower();

void processPower()
{
  if(millis() > timeSinceLastPowerCheck + powerCheckTime) 
  {
    checkPower();
    sendPower();
  }
}

void checkPower()
{
  currentPower = readvdd33()*1.000f;
  timeSinceLastPowerCheck = millis();

  charging = (currentPower == 0);
}


void sendPower()
{
   Serial.println(currentPower);
}

