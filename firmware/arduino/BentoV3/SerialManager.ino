void processSerial()
{
  while(Serial.available()>0)

  {
    char c = Serial.read();
    switch(c)
    {
      case 'p':
        #if USE_POWER
          sendPower();
        #endif
        break;
    }
  }
}

