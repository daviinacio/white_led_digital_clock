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

#include "config.hpp"
#include "Arduino.h"

// Drivers
#include "drivers/Display.hpp"
#include "drivers/RTC.hpp"
#include "drivers/DHT.hpp"
#include "drivers/Panel.hpp"

// Hardware
AnalogPanel panel = AnalogPanel(A3, 100);

// Core
#include "lib/ScreenController.h"

// Screens
#include "view/HomeScreen.hpp"
#include "view/ChronometerScreen.hpp"
#include "view/AdjustClockScreen.hpp"

ThreadController cpu;
ScreenController screen_controller;

HomeScreen screen_home;
ChronometerScreen screen_chronometer;
AdjustClockScreen screen_adjust_clock;

void setup() {
  // Hardware Threads
  cpu.add(&display);
  cpu.add(&rtc);
  cpu.add(&dht);
  cpu.add(&panel);

  // Screen Threads
  screen_controller.add(&screen_home);
  screen_controller.add(&screen_chronometer);
  screen_controller.add(&screen_adjust_clock);
  cpu.add(&screen_controller);

  // Driver Begins
  display.begin();
  rtc.begin();
  dht.begin();

  panel.addEventListener(&screen_controller);

  // Boot screen
  display.print(WLDC_SPLASH_TEXT);
  screen_controller.navigate<HomeScreen>();

  delay(WLDC_SETUP_DELAY);
}

void loop() {
  cpu.run();
}
