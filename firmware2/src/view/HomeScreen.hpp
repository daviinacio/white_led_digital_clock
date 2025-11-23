#include "_define.h"
#include "../lib/Screen.h"
#include "../drivers/Display.hpp"
#include "../drivers/RTC.hpp"

#ifndef WLDC_HOME_SCREEN_H
#define WLDC_HOME_SCREEN_H

class HomeScreen : public Screen {
public:
  HomeScreen(){
    ThreadID = SCREEN_HOME;
    interval = 1000;
  }

  void onRender() override {
    unsigned long m = millis();

    // Initial interval
    if(m <= 2000){ /* Do nothing */ }
      
    // // Temperature                  // Each 20s, runs on 16s, per 2s
    // if(((m / 2000 % 10 == 8 && main_cursor == 0 && idle) || main_cursor == 1) && dht_temp_buffer.getAverage() != DHT_INIT_VALUE){
    //   Display.setCursor(0);
    //   Display.print((int) dht_temp_buffer.getAverage());
    //   Display.printEnd(F("*C"));
    // } else
    
    // // Humidity                     // Each 20s, runs on 18s, per 2s
    // if(((m / 2000 % 10 == 9 && main_cursor == 0 && idle) || main_cursor == 2) && dht_hum_buffer.getAverage() != DHT_INIT_VALUE){
    //   Display.clear();
    //   Display.setCursor(0);
    //   Display.print(F("H"));
    //   Display.printEnd(dht_hum_buffer.getAverage());
    // }
    
    // Hours and Minutes
    else {                          // Runs when others 'IFs' are false
      Display.setTimeSeparator(RTC.enabled);
      Display.setCursor(0);
      
      if(RTC.now.hour() < 10)
        Display.print(0);
      Display.print(RTC.now.hour());

      if(RTC.now.minute() < 10)
        Display.print(0);
      Display.print(RTC.now.minute());
    }
  }
};

#endif
