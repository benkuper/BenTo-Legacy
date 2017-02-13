#ifndef TOUCH_MANAGER_H
#define TOUCH_MANAGER_H

#include <Seeed_QTouch.h>

class TouchManager
{
public :

  #define NUM_TOUCH 7
  boolean lastTouch[NUM_TOUCH];
  boolean touch[NUM_TOUCH];
  
  TouchManager()
  {
  }
  void init()
  {
    for(int i=0;i<NUM_TOUCH;i++)
    {
      lastTouch[i] = false;
      touch[i] = false;
    }
  }
  
  void update()
  {
    
    for(int i=0;i<NUM_TOUCH;i++)
    {
      touch[i] = QTouch.isTouch(i);
      if(touch[i] != lastTouch[i])
      {
        onTouchUpdate(i,touch[i]);
        lastTouch[i] = touch[i];
      }
    }
    
  }

  typedef void(*onEvent)(int,bool);
  void (*onTouchUpdate) (int,bool);
  
  void addCallbackTouchUpdate (onEvent func) { 
    onTouchUpdate = func;
  }
};

#endif
