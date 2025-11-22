#include "_def.h"
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
    Serial.println("ChronometerScreen.onStart");

  }

  void onRender() override {
    Serial.println("ChronometerScreen.onRender");

    if(millis() / 5000 % 2 == 0)
      navigate(SCREEN_HOME);
  }

  void onStop() override {
    Serial.println("ChronometerScreen.onStop");
  }

  void onKeyDown() override {
    Serial.println("ChronometerScreen.onKeyDown");
  }

  void onKeyPress() override {
    Serial.println("ChronometerScreen.onKeyPress");
  }

  void onKeyUp() override {
    Serial.println("ChronometerScreen.onKeyUp"); 
  }
};

#endif
