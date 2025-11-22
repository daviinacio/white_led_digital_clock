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

#include "Arduino.h"

// #include "drivers/Display.hpp"
#include "lib/ScreenController.h"
#include "view/HomeScreen.hpp"
#include "view/ChronometerScreen.hpp"

ScreenController scr_ctrl;

HomeScreen scr_home = HomeScreen();
ChronometerScreen scr_chronometer = ChronometerScreen();

void setup() {
  Serial.begin(9600);
  Serial.println("Firmware 2.0");

  scr_ctrl.add(&scr_home);
  scr_ctrl.add(&scr_chronometer);

  scr_ctrl.navigate(SCREEN_HOME);
}

void loop() {
  scr_ctrl.run();
}
