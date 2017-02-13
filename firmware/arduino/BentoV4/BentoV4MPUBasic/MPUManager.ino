RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object


void setupMPU()
{
  int errcode;
  
  
  imu = RTIMU::createIMU(&settings);                        // create the imu object
  
  Serial.print("ArduinoIMU starting using device "); Serial.println(imu->IMUName());
  if ((errcode = imu->IMUInit()) < 0) {
    Serial.print("Failed to init IMU: "); Serial.println(errcode);
  }
  
  if (imu->getCalibrationValid())
    Serial.println("Using compass calibration");
  else
    Serial.println("No valid compass calibration data");

  // Slerp power controls the fusion and can be between 0 and 1
  // 0 means that only gyros are used, 1 means that only accels/compass are used
  // In-between gives the fusion mix.
    
  fusion.setSlerpPower(0.02);
    
  // use of sensors in the fusion algorithm can be controlled here
  // change any of these to false to disable that sensor
  
  fusion.setGyroEnable(true);
  fusion.setAccelEnable(true);
  fusion.setCompassEnable(true);
}

void updateMPU()
{
  int loopCount = 1;
  
  while (imu->IMURead()) {                                // get the latest data if ready yet
    // this flushes remaining data in case we are falling behind
    if (++loopCount >= 10)
      continue;
    fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
  }  
  
  RTVector3 pose = fusion.getFusionPose(); //pose : roll, pitch, yaw
  ypr[0] = -pose.z(); 
  ypr[1] = pose.y();
  ypr[2] = -pose.x();
  
}
