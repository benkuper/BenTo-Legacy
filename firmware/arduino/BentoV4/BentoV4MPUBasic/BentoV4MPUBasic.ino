#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "CalLib.h"

float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void setup() {
  // put your setup code here, to run once:
  Wire.begin(2,4);
  Serial.begin(115200);
  setupMPU();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateMPU();

  sendOrientationData();

  delay(10);
}

void sendOrientationData()
{
   float yaw = -ypr[0]*180/M_PI;
  float pitch = ypr[1]*180/M_PI;
  float roll = -ypr[2]*180/M_PI;
  sendFloat3("o", yaw, pitch, roll);
}


void sendFloat3(char addr[], float val1, float val2, float val3){
  Serial.print(addr);
  Serial.print(' ');
  Serial.print(val1);
  Serial.print(' ');
  Serial.print(val2);
  Serial.print(' ');
  Serial.println(val3);
} 
