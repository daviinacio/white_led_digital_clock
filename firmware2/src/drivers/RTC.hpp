#include "Arduino.h"
#include <Thread.h>
#include <avr/io.h>
#include "RTClib.h"
#include "Display.hpp"

#ifndef WLDC_RTC_DRIVER_H
#define WLDC_RTC_DRIVER_H

class RealTimeClockDriver : public Thread {
protected:
  RTC_DS1307 rtc;

public:
  RealTimeClockDriver();
  void begin();
  void run();

  DateTime now;
};

RealTimeClockDriver::RealTimeClockDriver(){
  setInterval(1000);
}

void RealTimeClockDriver::begin(){
  while(!rtc.begin()){
    // Blink error for five seconds
    if(millis()/500 % 3 >= 1){
      display.setCursor(0);
      display.print(F("ERRO"));
    } else {
      display.clear();
    }
  }
}

void RealTimeClockDriver::run(){
  now = rtc.now();
}

RealTimeClockDriver rtc;

#endif
