#include "Screen.h"
#include "ScreenController.h"

Screen::Screen(unsigned short _id) : Screen(_id, SCREEN_DEFAULT_INTERVAL){}

Screen::Screen(unsigned short _id, unsigned short _render_interval){
  Id = _id;
  render_interval = _render_interval;
}

void Screen::attachController(ScreenController *c){
  controller = c;
}

void Screen::navigate(ScreenID screen_id) {
  if (controller && controller->activeScreen == this)
      controller->navigate(screen_id);
}
