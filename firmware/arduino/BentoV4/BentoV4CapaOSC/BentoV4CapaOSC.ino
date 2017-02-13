#include <Wire.h>

#include "WifiManager.h"
#include "OSCManager.h"
#include "TouchManager.h"

WifiManager wifiManager;
OSCManager oscManager;
TouchManager touchManager;

//predeclaration
void touchUpdate(int touchID, bool touched);


void setup() {
  // put your setup code here, to run once:
  Wire.begin(2,4);
  Serial.begin(115200);

  wifiManager.init();
  oscManager.init(&wifiManager.udp);
  touchManager.init();

  touchManager.addCallbackTouchUpdate(&touchUpdate);
}


void loop() {
  // put your main code here, to run repeatedly:

  wifiManager.update();
  oscManager.update();
  touchManager.update();

  delay(10);
}


void touchUpdate(int touchID, bool touched)
{
  Serial.print("Touch update : ");
  Serial.print(touchID);
  Serial.print(" > ");
  Serial.println(touched);


  OSCMessage msg("/touch");
  msg.add(touchID);
  msg.add(touched);
  oscManager.sendMessage(msg,"192.168.173.1",10000);
  
}

