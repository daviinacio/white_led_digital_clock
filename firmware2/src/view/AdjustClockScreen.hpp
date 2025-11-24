#include "../lib/Screen.h"
#include "../drivers/Display.hpp"
#include "../drivers/RTC.hpp"

#include "RTClib.h"

#ifndef WLDC_ADJUST_CLOCK_SCREEN_H
#define WLDC_ADJUST_CLOCK_SCREEN_H

class AdjustClockScreen : public Screen<AdjustClockScreen> {
protected:
  unsigned short cursor = 0;
  DateTime time;

public:
  static constexpr ScreenID Id = 0x03;

  void onStart() override {
    display.setCursor(0);
    display.print(F("A 01"));


    time = rtc.now;
    display.clear();
  }

  void onRender() override {
    display.setCursor(0);
    display.print(F("A 02"));
    // display.setCursor(0);
    
    // if(cursor == 0 || cursor == 1){  // Minutes & Hours
    //   display.setCursor(0);

    //   if((cursor == 1 && millis() / DISP_BRINK_INTERVAL % 3 == 0) && cursor_blink){   // Blink 1/3 on focus
    //     display.print(F("  "));
    //   }
    //   else {
    //     if(time_adjust_hour < 10)
    //       display.print(0);
    //     display.print(time_adjust_hour);
    //   }

    //   if((cursor == 0 && millis()/DISP_BRINK_INTERVAL % 3 == 0) && cursor_blink){   // Blink 1/3 on focus
    //     display.print(F("  "));
    //   }
    //   else {
    //     if(time_adjust_minute < 10)
    //       display.print(0);
    //     display.print(time_adjust_minute);
    //   }
    // }
    // else
    // if(cursor == 2){  // Day
    //   display.setCursor(0);
    //   display.print(F("D "));

    //   if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && cursor_blink){  // Blink 1/3 on focus
    //     display.print(F("  "));
    //   }
    //   else {
    //     if(time_adjust_day % 100 < 10)
    //       display.print(F(" "));
          
    //     display.printEnd(time_adjust_day);
    //   }
    // }
    // else
    // if(cursor == 3){  // Month
    //   display.setCursor(0);
    //   display.print(F("M "));

    //   if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && cursor_blink){  // Blink 1/3 on focus
    //     display.print(F("  "));
    //   }
    //   else {
    //     if(time_adjust_month % 100 < 10)
    //       display.print(F(" "));
          
    //     display.printEnd(time_adjust_month);
    //   }
    // }
    // else
    // if(cursor == 4){  // Year
    //   display.setCursor(0);
    //   display.print(F("Y "));

    //   if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && cursor_blink){  // Blink 1/3 on focus
    //     display.print(F("  "));
    //   }
    //   else {
    //     if(time_adjust_year % 100 < 10)
    //       display.print(F(" "));
          
    //     display.printEnd((int) time_adjust_year % 100);
    //   }
    // }
  }

  void onStop() override {
    display.setCursor(0);
    display.print(F("A 03"));
  }
};

#endif
