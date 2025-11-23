/*
 * (c) DaviApps 2025
 * 
 * White LED Digital Clock 
 * 
 * description: This is the firmware of a led digital clock (7 segments)
 * author: https://github.com/daviinacio
 * date: 22/11/2025
 * 
 */

#define WLDC_DISPLAY_DEBUG_MODE false

#include "Arduino.h"

// Drivers
#include "drivers/Display.hpp"
#include "drivers/RTC.hpp"
#include "drivers/DHT.hpp"

// Core
#include "lib/ScreenController.h"

// Screens
#include "view/HomeScreen.hpp"
#include "view/ChronometerScreen.hpp"

ThreadController cpu;
ScreenController screen_controller;

HomeScreen scr_home;
ChronometerScreen scr_chronometer;

void setup() {
  // Hardware Threads
  cpu.add(&display);
  cpu.add(&rtc);
  cpu.add(&dht);

  // Screen Threads
  screen_controller.add(&scr_home);
  screen_controller.add(&scr_chronometer);
  cpu.add(&screen_controller);

  // Driver Begins
  display.begin();
  rtc.begin();
  dht.begin();
  //dht.setInterval(4000);

  // Boot screen
  display.print(F("DAVI"));
  screen_controller.navigate(SCREEN_HOME);

  delay(1000);
}

void loop() {
  cpu.run();
}
