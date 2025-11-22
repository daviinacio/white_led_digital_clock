#include "../lib/screen.hpp"
#include "../drivers/display.hpp"

#ifndef WLDC_HOME_SCREEN_H
#define WLDC_HOME_SCREEN_H

class HomeScreen : Screen<HomeScreen> {
public:
  void onCreate() override {
    refreshInterval = 1000;
  }

  void onRender() override {
    
  }

  void onDestroy() override {
  }
};

#endif
