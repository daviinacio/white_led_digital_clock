#include "ScreenController.h"

bool ScreenController::add(Screen* _screen){
  _screen->enabled = false;
  _screen->attachController(this);
  return ThreadController::add(_screen);
}

void ScreenController::navigate(int screen_id){
    if(_active_screen_index != MAX_THREADS)
      ((Screen*) thread[_active_screen_index])->onStop();

    int checks = 0;
    for(int i = 0; i < MAX_THREADS && checks <= cached_size; i++){
      if(thread[i]){
        checks++;
        thread[i]->enabled = thread[i]->ThreadID == screen_id;
        if(thread[i]->enabled)
          _active_screen_index = i;
      }
    }

    ((Screen*) thread[_active_screen_index])->onStart();
  }
