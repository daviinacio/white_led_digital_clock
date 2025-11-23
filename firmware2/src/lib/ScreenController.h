#ifndef WLDC_SCREEN_CONTROLLER_CLASS_H
#define WLDC_SCREEN_CONTROLLER_CLASS_H

#define MAX_THREADS		15

#include "Screen.h"
#include <ThreadController.h>

class ScreenController : public ThreadController {
protected:
  unsigned short _active_screen_index = MAX_THREADS;

public:
  bool add(Screen* _screen);
  void navigate(int screen_id);
};


#endif
