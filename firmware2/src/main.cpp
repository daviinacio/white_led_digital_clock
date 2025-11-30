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

#include "config.h"
#include "utils.h"
#include "Arduino.h"

// Assets
#include "assets/music/sebastian_bach/bourree.hpp"
#include "assets/music/coca_cola/theme.hpp"
#include "assets/music/unknown/la_cucaracha.hpp"
#include "assets/music/wintergatan/marble_machine.hpp"

// Drivers
#include "drivers/Display.h"
#include "drivers/RTC.hpp"
#include "drivers/DHT.hpp"
#include "drivers/Panel.hpp"
#include "drivers/Buzzer.hpp"

// Core
#include "lib/ScreenController.h"
#include "lib/MusicPlayer.hpp"

// Screens
#include "view/HomeScreen.hpp"
#include "view/ChronometerScreen.hpp"
#include "view/AdjustClockScreen.hpp"
#include "view/BrightnessScreen.hpp"
#include "view/MusicScreen.hpp"

ThreadController cpu;
ScreenController screen_controller;

HomeScreen home_screen;
ChronometerScreen chronometer_screen;
AdjustClockScreen adjust_clock_screen;
BrightnessScreen brightness_screen;
MusicScreen music_screen;

void key_press(InputKey key, unsigned int milliseconds);

void setup() {
  // Hardware Threads
  cpu.add(&display);
  // cpu.add(&dht);
  cpu.add(&panel);
  cpu.add(&player);
  // cpu.add(&buzzer);

  // Screens
  screen_controller.add(&home_screen);
  screen_controller.add(&chronometer_screen);
  screen_controller.add(&brightness_screen);
  screen_controller.add(&adjust_clock_screen);
  screen_controller.add(&music_screen);

  // Screen Threads
  cpu.add(&chronometer_screen);
  cpu.add(&screen_controller);

  // Driver Begins
  rtc.begin();
  dht.begin();
  display.begin();
  buzzer.begin();

  panel.addEventListener(&screen_controller);
  panel.onKeyPress(key_press);

  // player.playSync(sebastian_bach__bourree, 0);

  // Boot screen
  display.enable();
  display.print(WLDC_SPLASH_TEXT);
  delay(WLDC_SETUP_DELAY);

  screen_controller.navigate(WLDC_SCREEN_MUSIC);
}

void loop() {
  cpu.run();
  // buzzer.run();
}

void key_press(InputKey key, unsigned int milliseconds){
  if(milliseconds < PANEL_LONG_PRESS || key != InputKey::KEY_HOME) return;
  display.printScroll(F("----"), 1000);
  screen_controller.navigate(WLDC_SCREEN_HOME);
}

// TIMER2 Interrupt
ISR(TIMER2_COMPA_vect){
  display.run_multiplex();
  // if(buzzer.shouldRun(millis()))
    
}
