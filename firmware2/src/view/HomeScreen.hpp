#include "../lib/Screen.h"

#include "../drivers/Display.h"
#include "../drivers/RTC.hpp"
#include "../drivers/DHT.hpp"

#include "ChronometerScreen.hpp"

#ifndef WLDC_HOME_SCREEN_H
#define WLDC_HOME_SCREEN_H

class HomeScreen : public Screen {
private:
  unsigned short cursor = 0;

public:
  static constexpr ScreenID Id = 1;

  HomeScreen() : Screen(Id){}

  void render() override {
    DateTime now = rtc.now();
    bool isIdle = input->isIdle(500);
    unsigned long m = millis();

    // Initial interval
    if(m <= 2000){ /* Do nothing */ }
      
    // Temperature                  // Each 20s, runs on 16s, per 2s
    if(((m / 2000 % 10 == 8 && cursor == 0 && isIdle) || cursor == 1) && dht.getTemperature() != DHT_INIT_VALUE){
      display.setTimeSeparator(false);
      display.setCursor(0);
      display.print((int) dht.getTemperature());
      display.printEnd(F("*C"));
    } else
    
    // Humidity                     // Each 20s, runs on 18s, per 2s
    if(((m / 2000 % 10 == 9 && cursor == 0 && isIdle) || cursor == 2) && dht.getHumidity() != DHT_INIT_VALUE){
      display.setTimeSeparator(false);
      display.clear();
      display.setCursor(0);
      display.print(F("H"));
      display.printEnd(dht.getHumidity());
    }
    
    // Hours and Minutes
    else {
      display.setTimeSeparator(true);
      display.setCursor(0);
      
      if(now.hour() < 10)
        display.print(0);
      display.print(now.hour());

      if(now.minute() < 10)
        display.print(0);
      display.print(now.minute());
    }
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME && !display.isScrolling()){
        navigate(ChronometerScreen::Id);
    }
  }

  bool keyDown(InputKey key) override {
    if(key == KEY_VALUE_UP)
      display.incrementBrightness();
    else if(key == KEY_VALUE_DOWN)
      display.decrementBrightness();
    else if(key == InputKey::KEY_FUNC_RIGHT)
      increment(cursor, 0, 2, false);
    else if(key == InputKey::KEY_FUNC_LEFT)
      decrement(cursor, 0, 2, false);

    render();
    return true;
  }
};

#endif
