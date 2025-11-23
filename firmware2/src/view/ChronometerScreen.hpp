#include "_define.h"
#include "../lib/Screen.h"
#include "../drivers/Display.hpp"

#ifndef WLDC_CHRONOMETER_SCREEN_H
#define WLDC_CHRONOMETER_SCREEN_H

class ChronometerScreen : public Screen {
public:
  ChronometerScreen(){
    ThreadID = SCREEN_CHRONOMETER;
  }

  void onStart() override {
    display.setCursor(0);
    display.print(F("C 01"));
  }

  void onRender() override {
    display.setCursor(0);
    display.print(F("C 02"));

    if(millis() / 5000 % 2 == 0)
      navigate(SCREEN_HOME);
  }

  void onStop() override {
    display.setCursor(0);
    display.print(F("C 03"));
  }
};

#endif
