#include "../lib/Input.hpp"

#ifndef WLDC_SCREEN_CONTROLLER_CLASS_H
#define WLDC_SCREEN_CONTROLLER_CLASS_H

#define MAX_THREADS		15

#include "Screen.h"
#include <ThreadController.h>

class ScreenController : public ThreadController, public InputListener {
protected:
  static ScreenController* instance;
  ScreenBase* activeScreen = nullptr;

public:
  void onKeyPress(InputKey key, unsigned int milliseconds) override;
  void onKeyUp(InputKey key, unsigned int milliseconds) override;
  bool onKeyDown(InputKey key) override;

  bool add(ScreenBase* _screen);
  void navigate(ScreenID screen_id);
  
  template <typename T>
  void navigate() {
      navigate(T::Id);
  }
};


#endif
