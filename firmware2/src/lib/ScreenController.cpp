#include "ScreenController.h"

// ScreenController::ScreenController(){
//   instance = this;
// }

void ScreenController::onKeyPress(InputKey key, unsigned int milliseconds) {
  if(activeScreen)
    activeScreen->onKeyPress(key, milliseconds);
}

void ScreenController::onKeyUp(InputKey key, unsigned int milliseconds) {
  if(activeScreen)
    activeScreen->onKeyPress(key, milliseconds);
}

bool ScreenController::onKeyDown(InputKey key) {
  if(activeScreen)
    return activeScreen->onKeyDown(key);
  return false;
}

bool ScreenController::add(ScreenBase *_screen)
{
  _screen->enabled = false;
  _screen->attachController(this);
  return ThreadController::add(_screen);
}

void ScreenController::navigate(ScreenID screen_id){
  if(activeScreen) activeScreen->onStop();

  for(Thread* thr : thread){
    if(!thr) continue;
    thr->enabled = thr->ThreadID == screen_id;
    if(thr->enabled)
      activeScreen = (ScreenBase*) thr;
  }

  if(activeScreen) activeScreen->onStart();
}
