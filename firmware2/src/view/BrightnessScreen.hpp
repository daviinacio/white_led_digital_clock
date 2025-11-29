#include "assets/music/base.h"

class BrightnessScreen : public Screen {
private:

public:
  BrightnessScreen() : Screen(WLDC_SCREEN_BRIGHTNESS, 250){}

  void start() override {
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
        navigate(WLDC_SCREEN_MUSIC);
      else
        navigate(1);
    }
  }

  bool keyDown(InputKey key) override {
    if(display.isScrolling()) return true;

    if(key == KEY_VALUE_UP)
      display.incrementBrightness();
    else if(key == KEY_VALUE_DOWN)
      display.decrementBrightness();

    render();
    return true;
  }

  void keyPress(InputKey key, unsigned int milliseconds){
    if(milliseconds < PANEL_LONG_PRESS) return;
    if(key == InputKey::KEY_VALUE_UP || key == InputKey::KEY_VALUE_DOWN){
      display.autoBrightness();
      display.printScroll(F("AUTO"), 1500);
    }
  }
};
