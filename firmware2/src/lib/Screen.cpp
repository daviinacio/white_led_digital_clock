#include "Screen.h"
#include "ScreenController.h"

Screen::Screen(){
  interval = 500;
}

void Screen::navigate(int screen_id){
  if (controller)
    controller->navigate(screen_id);
}

void Screen::attachController(ScreenController* c) {
    controller = c;
}

void Screen::run(){
  onRender();
  runned();
}
