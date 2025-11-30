#include "Input.hpp"
#include "drivers/Display.h"

#ifndef WLDC_SCREEN_CONTROLLER_CLASS_H
#define WLDC_SCREEN_CONTROLLER_CLASS_H

#define MAX_SCREENS 15

#include "Screen.h"
#include <ThreadController.h>

class ScreenController : public Thread, public InputListener {
protected:
  unsigned short activeScreenIndex = 0;
  Screen* screens[MAX_SCREENS];

  void run() override;

public:
  Screen* activeScreen = nullptr;

  ScreenController();

  void keyPress(InputKey key, unsigned int milliseconds) override;
  void keyUp(InputKey key, unsigned int milliseconds) override;
  bool keyDown(InputKey key) override;

  bool add(Screen* _screen);
  void navigate(ScreenID screen_id);

  void attachInput(Input* _input) override;
};


#endif
