////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014-2015, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <Wire.h>
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "CalLib.h"
#include <EEPROM.h>

RTIMU *imu;                                           // the IMU object
RTIMUSettings settings;                               // the settings object
CALLIB_DATA calData;                                  // the calibration data

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

#define USE_BLUETOOTH 0

#if USE_BLUETOOTH
  #define serial Serial1
#else
  #define serial Serial
#endif


void resetCalibData()
{
  for (int i = 0; i < 3; i++) {
    calData.magMin[i] = 10000000;                    // init mag cal data
    calData.magMax[i] = -10000000;
  }
}

void saveCalib()
{
  calData.magValid = true;
  calLibWrite(0, &calData);
  
  serial.print("Mag cal data saved for device "); 
  serial.println(imu->IMUName());
}

void setup()
{
  
  calLibRead(0, &calData);                           // pick up existing mag data if there   

  calData.magValid = false;
  resetCalibData();
   
  serial.begin(SERIAL_PORT_SPEED);
  delay(500);
  serial.println("ArduinoMagCal starting");
  serial.println("Enter s to save current data to EEPROM");

  Wire.begin(2,4);
   
  imu = RTIMU::createIMU(&settings);                 // create the imu object
  imu->IMUInit();
  imu->setCalibrationMode(true);                     // make sure we get raw data
  serial.print("ArduinoIMU calibrating device "); Serial.println(imu->IMUName());
}



void loop()
{  
  boolean changed;
  RTVector3 mag;
  
  if (imu->IMURead()) {                                 // get the latest data
    changed = false;
    mag = imu->getCompass();
    for (int i = 0; i < 3; i++) {
      if (mag.data(i) < calData.magMin[i]) {
        calData.magMin[i] = mag.data(i);
        changed = true;
      }
      if (mag.data(i) > calData.magMax[i]) {
        calData.magMax[i] = mag.data(i);
        changed = true;
      }
    }
 
    if (changed) {
      serial.println("-------");
      serial.print("minX: "); serial.print(calData.magMin[0]);
      serial.print(" maxX: "); serial.print(calData.magMax[0]); serial.println();
      serial.print("minY: "); serial.print(calData.magMin[1]);
      serial.print(" maxY: "); serial.print(calData.magMax[1]); serial.println();
      serial.print("minZ: "); serial.print(calData.magMin[2]);
      serial.print(" maxZ: "); serial.print(calData.magMax[2]); serial.println();
    }
  }else
  {
    //Serial.println("Couldn't read IMU");
  }
  
  if (serial.available()) {
    char c = serial.read();
    switch(c)
    {
      case 's':// save the data
      saveCalib();
      break;

      case 'r':
      resetCalibData();
      break;
    }
  }

  delay(50);
}
