int ledR, ledG, ledB;

void setLed(byte r,byte g, byte b);

void setupLed()
{
  //Serial.println("InitLed");
  Wire.beginTransmission(98);    
  
  Wire.write(0x80); //Control reg : write to all registers
  Wire.write(0x80); //MODE1 reg : normal mode (stop sleeping)
  Wire.write(0x01); //MODE2 reg : no change
  Wire.write(0x00); //PWM0
  Wire.write(0x00); //PWM1
  Wire.write(0x00); //PWM2
  Wire.write(0x00); //PWM3
  Wire.write(0xFF); //GRPPWM
  Wire.write(0x00); //GRPFREQ
  Wire.write(0xFF); //LEDOUT - all outputs controlled by PWM and GRP
  
  Wire.endTransmission();

  //blink 3 times
  for(int i=0;i<3;i++)
  {
    
    setLed(20,20,20);
    delay(30);
    setLed(0,0,0);
    delay(30);
    setLed(20,20,20);
    delay(30);
  }
  
  setLed(0,0,0);
}


void updateLed()
{
  //Serial.println("Led");
  Wire.beginTransmission(98);
  Wire.write(0x82); //Control reg : write to PWM registers
  Wire.write(ledR); //PWM0
  Wire.write(ledG); //PWM1
  Wire.write(ledB); //PWM2
  Wire.write(0xFF); //PWM3
  Wire.endTransmission();
}

//intensity : [0-255]
void setLed(byte r, byte g, byte b)
{
  ledR = r;
  ledG = g;
  ledB = b;
  updateLed();
}


