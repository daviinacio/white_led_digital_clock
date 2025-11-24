#include "Screen.h"
#include "ScreenController.h"
#include "screen.h"

void ScreenBase::attachController(ScreenController *c){
  controller = c;
}

void ScreenBase::run() {
  onRender();
  runned();
}

void ScreenBase::navigate(ScreenID screen_id) {
  if (controller)
      controller->navigate(screen_id);
}
