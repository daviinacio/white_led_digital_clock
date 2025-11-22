#include "_def.h"
#include "../lib/Screen.h"
#include "../drivers/Display.hpp"

#ifndef WLDC_HOME_SCREEN_H
#define WLDC_HOME_SCREEN_H

class HomeScreen : public Screen {
public:
  HomeScreen(){
    ThreadID = SCREEN_HOME;
    interval = 1000;
  }

  void onStart() override {
    Serial.println("HomeScreen.onStart");
  }

  void onRender() override {
    Serial.println("HomeScreen.onRender");
    if(millis() / 5000 % 2 == 1)
      navigate(SCREEN_CHRONOMETER);
  }

  void onStop() override {
    Serial.println("HomeScreen.onStop");
  }
};

#endif
