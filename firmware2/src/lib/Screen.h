#include <Thread.h>
#include "ScreenController.h"

#ifndef WLDC_SCREEN_CLASS_H
#define WLDC_SCREEN_CLASS_H

class ScreenController;

// template <typename T>
class Screen : public Thread
{
protected:
  ScreenController* controller = nullptr;

  void navigate(int screen_id);

public:
  virtual ~Screen() {}
  Screen();
  
  virtual void onStart() = 0;
  virtual void onRender() = 0;
  virtual void onStop() = 0;

  virtual void onKeyDown(){};
  virtual void onKeyPress(){};
  virtual void onKeyUp(){};

  void run();

  void attachController(ScreenController* c);
};

#endif
