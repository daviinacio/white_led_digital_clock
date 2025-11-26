/*
 * (c) DaviApps 2025
 * 
 * White LED Digital Clock v2
 * 
 * description: This is the firmware of a led digital clock (7 segments)
 * author: https://github.com/daviinacio
 * date: 22/11/2025
 * 
 */

#include "config.hpp"
#include "utils.h"
#include "Arduino.h"

// Drivers
#include "drivers/Display.h"
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
#include "view/BrightnessScreen.hpp"

ThreadController cpu;
ScreenController screen_controller;

HomeScreen home_screen;
ChronometerScreen chronometer_screen;
AdjustClockScreen adjust_clock_screen;
BrightnessScreen brightness_screen;

void key_up(InputKey key, unsigned int milliseconds);
void key_press(InputKey key, unsigned int milliseconds);

void setup() {
  // Hardware Threads
  cpu.add(&display);
  cpu.add(&dht);
  cpu.add(&panel);

  // Screens
  screen_controller.add(&home_screen);
  screen_controller.add(&chronometer_screen);
  screen_controller.add(&brightness_screen);
  screen_controller.add(&adjust_clock_screen);

  // Screen Threads
  cpu.add(&chronometer_screen);
  cpu.add(&screen_controller);

  // Driver Begins
  display.begin();
  rtc.begin();
  dht.begin();

  panel.addEventListener(&screen_controller);
  panel.onKeyUp(key_up);
  panel.onKeyPress(key_press);

  // Boot screen
  display.print(WLDC_SPLASH_TEXT);
  delay(WLDC_SETUP_DELAY);
}

void loop() {
  cpu.run();
}

void key_up(InputKey key, unsigned int milliseconds){
  // if(display.isScrolling() && key == KEY_HOME){
  //   screen_controller.navigateNext();
  // }
}

void key_press(InputKey key, unsigned int milliseconds){
  if(milliseconds < PANEL_LONG_PRESS || key != InputKey::KEY_HOME) return;
  screen_controller.navigate(HomeScreen::Id);
  display.clearScroll();
  display.printScroll(F("----"), 1000);
}
