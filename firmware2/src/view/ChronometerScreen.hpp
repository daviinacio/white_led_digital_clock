#include "../lib/Screen.h"
#include "../drivers/Display.hpp"

#ifndef WLDC_CHRONOMETER_SCREEN_H
#define WLDC_CHRONOMETER_SCREEN_H

class ChronometerScreen : public Screen<ChronometerScreen> {
public:
  static constexpr ScreenID Id = 0x02;

  void onStart() override {
    display.setCursor(0);
    display.print(F("C 01"));
  }

  void onRender() override {
    display.setCursor(0);
    display.print(F("C 02"));
  }

  void onStop() override {
    display.setCursor(0);
    display.print(F("C 03"));
  }
};

#endif
