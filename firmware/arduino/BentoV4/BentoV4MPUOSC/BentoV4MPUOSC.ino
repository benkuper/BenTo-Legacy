#include <Wire.h>
#include "MPUManager.h"

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include "WifiManager.h"

#include <OSCMessage.h>
#include <OSCBoards.h>

void sendMessage(OSCMessage &msg)
{
  udp.beginPacket("192.168.173.1",10000);
  msg.send(udp);
  udp.endPacket();
  msg.empty();
}

void sendFloat3(char addr[], float val1, float val2, float val3){
  Serial.print(addr);
  Serial.print(' ');
  Serial.print(val1);
  Serial.print(' ');
  Serial.print(val2);
  Serial.print(' ');
  Serial.println(val3);

  OSCMessage msg("/orientation");
  msg.add(val1);
  msg.add(val2);
  msg.add(val3);
  sendMessage(msg);
} 

void sendOrientationData()
{
  float yaw = -ypr[0]*180/M_PI;
  float pitch = ypr[1]*180/M_PI;
  float roll = -ypr[2]*180/M_PI;
  sendFloat3("o", yaw, pitch, roll);
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin(2,4);
  Serial.begin(115200);

  initWifi();
  setupMPU();
}


void loop() {
  // put your main code here, to run repeatedly:
  updateMPU();
  sendOrientationData();

  delay(10);
}


