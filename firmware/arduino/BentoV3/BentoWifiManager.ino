
bool sendRate = 30; //ms

void initWifi()
{ 
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");  
  
  int redIndex = 1;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("wait... ");
    Serial.println(redIndex);
    setRangeColor(0,redIndex,COLOR_RED,true);
    redIndex = (redIndex+1)%LEDS_PER_STRIP;
  }

  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  setFullColor(COLOR_BLUE);
  delay(500);
  
  Serial.print("Setting up UDP...");
  
   udp.begin(8888);
   Serial.println("OK");
   
  Serial.print("Registering mDNS with name ");
  Serial.print(bonjourName);
  Serial.print("...");
  
  if (!MDNS.begin(bonjourName)) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  Serial.print("OK : ");
  Serial.print("Registering Bonjour service...");
  
  MDNS.addService("ledcontrol", "udp", 8888);
  
  Serial.println("OK");

  setFullColor(COLOR_GREEN);
  delay(1000);
}

void processBuffer();

void processWifi()
{
  if (udp.parsePacket())
  {
    while (udp.available() > 0)
    {
      byte b = udp.read();

      if (b == 255) processBuffer();
      else if (ledBufferIndex < NUM_ACTIVE_LEDS)
      {
        colorBuffer[colorBufferIndex] = (uint8_t)b;
        colorBufferIndex++;
        if (colorBufferIndex == 3)
        {
          setLed(ledBufferIndex, colorBuffer[0], colorBuffer[1], colorBuffer[2]);

          colorBufferIndex = 0;
          ledBufferIndex ++;
        }
      }
    }
    udp.flush();
  }

  delay(2);
  //udp.send("o 1.2543487 1.214657 0.312354589");
}

void processBuffer()
{
  //Serial.println("process Buffer !");
  strip.Show();
  ledBufferIndex = 0;
}

