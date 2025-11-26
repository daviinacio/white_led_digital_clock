#include "../lib/Screen.h"

#include "AdjustClockScreen.hpp"

#ifndef WLDC_BRIGHTNESS_SCREEN_H
#define WLDC_BRIGHTNESS_SCREEN_H

class BrightnessScreen : public Screen {
private:
  unsigned short cursor = 0;

public:
  static constexpr ScreenID Id = 3;

  BrightnessScreen() : Screen(Id, 250){}

  void start() override {
    // display.clearScroll();
    display.printScroll(F("    BRILHO    "));
  }

  void render() override {
    display.setCursor(0);
    display.print(F("BR"));

    if(display.getBrightness() < 10)
      display.print(F(" "));
    display.printEnd(display.getBrightness());
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(display.isScrolling())
        navigate(AdjustClockScreen::Id);
      else
        navigate(1);
    }
  }

  bool keyDown(InputKey key) override {
    if(key == KEY_VALUE_UP)
      display.incrementBrightness();
    else if(key == KEY_VALUE_DOWN)
      display.decrementBrightness();

    render();
    return true;
  }
};

#endif
