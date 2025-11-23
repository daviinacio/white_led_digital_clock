#include "_define.h"
#include "../lib/Screen.h"

#include "../drivers/Display.hpp"
#include "../drivers/RTC.hpp"
#include "../drivers/DHT.hpp"

#ifndef WLDC_HOME_SCREEN_H
#define WLDC_HOME_SCREEN_H

class HomeScreen : public Screen {
private:
  unsigned short cursor = 0;

public:
  HomeScreen(){
    ThreadID = SCREEN_HOME;
    interval = 1000;
  }

  void onRender() override {
    unsigned long m = millis();

    // Initial interval
    if(m <= 2000){ /* Do nothing */ }
      
    // Temperature                  // Each 20s, runs on 16s, per 2s
    if(((m / 2000 % 10 == 8 && cursor == 0 && isInputIdle) || cursor == 1) && dht.getTemperature() != DHT_INIT_VALUE){
      display.setTimeSeparator(false);
      display.setCursor(0);
      display.print((int) dht.getTemperature());
      display.printEnd(F("*C"));
    } else
    
    // Humidity                     // Each 20s, runs on 18s, per 2s
    if(((m / 2000 % 10 == 9 && cursor == 0 && isInputIdle) || cursor == 2) && dht.getHumidity() != DHT_INIT_VALUE){
      display.setTimeSeparator(false);
      display.clear();
      display.setCursor(0);
      display.print(F("H"));
      display.printEnd(dht.getHumidity());
    }
    
    // Hours and Minutes
    else {                          // Runs when others 'IFs' are false
      display.setTimeSeparator(true);
      display.setCursor(0);
      
      if(rtc.now.hour() < 10)
        display.print(0);
      display.print(rtc.now.hour());

      if(rtc.now.minute() < 10)
        display.print(0);
      display.print(rtc.now.minute());
    }
  }
};

#endif
