const char* ssid = "BenPortable";
const char* password = "benkuper";
const char* bonjourName = "BentoV4-1";

WiFiUDP udp;


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
    redIndex++;
  }

  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
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
  Serial.println("Registering Bonjour service...");
  
  MDNS.addService("ledcontrol", "udp", 8888);

  
  Serial.println("OK");

  delay(500);
}

