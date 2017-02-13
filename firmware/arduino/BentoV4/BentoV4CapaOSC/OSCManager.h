#ifndef OSC_MANAGER_H
#define OSC_MANAGER_H

#include "WifiManager.h"

#include <OSCMessage.h>
#include <OSCBoards.h>

class OSCManager
{
public:
  WiFiUDP * udp = NULL;
  
  OSCManager()
  {
    
  }
  
  void init(WiFiUDP * udp)
  {
    this->udp = udp;
  }
  
  void update()
  {
    
  }
  
  void sendMessage(OSCMessage & msg, const char * host, int port)
  {
    udp->beginPacket(host,port);
    msg.send(*udp);
    udp->endPacket();
    msg.empty();
  }
  
  typedef void(*onEvent)(OSCMessage &);
  void (*onMessageReceived) (OSCMessage &);
  
  void addCallbackMessageReceived (onEvent func) {
    onMessageReceived = func;
  }
  
};

#endif
