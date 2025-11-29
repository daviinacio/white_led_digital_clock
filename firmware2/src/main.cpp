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

// Drivers
#include "drivers/Display.h"
#include "drivers/RTC.hpp"
#include "drivers/DHT.hpp"
#include "drivers/Panel.hpp"
#include "drivers/Buzzer.hpp"

// Hardware
AnalogPanel panel = AnalogPanel(A3, 100);

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
  //cpu.add(&buzzer);

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

  // player.playSync(sebastian_bach__bourree);
  // player.playSync(coca_cola__theme);
  player.playSync(unknown__la_cucaracha);

  // Boot screen
  display.enable();
  display.print(WLDC_SPLASH_TEXT);
  delay(WLDC_SETUP_DELAY);
}

void loop() {
  cpu.run();
}

void key_press(InputKey key, unsigned int milliseconds){
  if(milliseconds < PANEL_LONG_PRESS || key != InputKey::KEY_HOME) return;
  display.printScroll(F("----"), 1000);
  screen_controller.navigate(WLDC_SCREEN_HOME);
}
