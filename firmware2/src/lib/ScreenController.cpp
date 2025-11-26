#include "ScreenController.h"

void ScreenController::run(){
  if(activeScreen && !display.isScrolling())
    activeScreen->render();
    
  Thread::run();
}

ScreenController::ScreenController(){
  interval = SCREEN_DEFAULT_INTERVAL;
}

void ScreenController::keyPress(InputKey key, unsigned int milliseconds){
  if(activeScreen)
    activeScreen->keyPress(key, milliseconds);
}

void ScreenController::keyUp(InputKey key, unsigned int milliseconds) {
  if(activeScreen)
    activeScreen->keyUp(key, milliseconds);
}

bool ScreenController::keyDown(InputKey key) {
  if(activeScreen)
    return activeScreen->keyDown(key);
  return true;
}

bool ScreenController::add(Screen *_screen){
  for(int i = 0; i < MAX_SCREENS; i++){
		if(!screens[i]){
      _screen->attachController(this);
      _screen->attachInput(input);
			screens[i] = _screen;
      if(i == 0){
        activeScreen = _screen;
        interval = activeScreen->render_interval;
      }
			return true;
		}
	}

  return false;
}

void ScreenController::navigate(ScreenID screen_id){
  if(activeScreen) {
    activeScreen->stop();
  }

  for(int i = 0; i < MAX_SCREENS; i++){
    Screen* scr = screens[i];
    if(!scr || scr->Id != screen_id) continue;

    activeScreen = scr;
    activeScreenIndex = i;
    interval = activeScreen->render_interval;
    break;
  }

  if(activeScreen) activeScreen->start();
}

void ScreenController::attachInput(Input *_input){
  for(Screen* screen : screens){
    if(!screen) continue;
    screen->attachInput(_input);
  }

  InputListener::attachInput(_input);
}
