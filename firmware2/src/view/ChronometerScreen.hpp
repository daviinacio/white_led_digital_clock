#include "../lib/Screen.h"
#include "../drivers/Display.h"
#include <Thread.h>

#include "BrightnessScreen.hpp"

#ifndef WLDC_CHRONOMETER_SCREEN_H
#define WLDC_CHRONOMETER_SCREEN_H

class ChronometerScreen : public Screen, public Thread {
protected:
  unsigned int counter = 0;

public:
  static constexpr ScreenID Id = 2;

  ChronometerScreen(): Screen(Id, 100), Thread(NULL, 100) {
    enabled = false;
  }

  void start() override {
    // display.clearScroll();
    display.printScroll(F("    CRONOMETRO    "));
  }

  void render() override {
    display.clear();

    if(counter < 600){
      display.setTimeSeparator(false);
      display.printEnd(counter * 0.1, 1);
    }
    else if(counter < 36000) {
      display.setCursor(0);
      unsigned short minute = (counter / 10) / 60;
      unsigned short seconds = (counter / 10) % 60;

      if(minute < 10)
        display.print(F(" "));
      display.print(minute);

      if(seconds < 10)
        display.print(0);
      display.print(seconds);
      
      display.setTimeSeparator((counter / 5 % 2 == 0) || !enabled);
    }
    else {
      enabled = false;
      counter = 0;
    }
  }

  void run() override {
    counter++;
    counter %= (int) pow(10, DISP_LENGTH);
    Thread::run();
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(display.isScrolling())
        navigate(BrightnessScreen::Id);
      else
        navigate(1);
    }
  }

  bool keyDown(InputKey key) override {
    if(key == KEY_VALUE_UP){
      enabled = !enabled;
      runned();
    }
    else if(key == KEY_VALUE_DOWN){
      if(enabled)
        enabled = false;
      else
        counter = 0;
    }

    render();
    return true;
  }
};

#endif
