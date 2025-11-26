#include <Thread.h>
#include "ScreenController.h"
#include "Input.hpp"

#define SCREEN_DEFAULT_INTERVAL 500

#ifndef WLDC_SCREEN_CLASS_H
#define WLDC_SCREEN_CLASS_H

class ScreenController;

using ScreenID = unsigned short;

class Screen : public InputListener {
private:
  ScreenController* controller = nullptr;

protected:
  void navigate(ScreenID screen_id);
  
public:
  ScreenID Id;
  unsigned short render_interval = 0;

  Screen(unsigned short _id);
  Screen(unsigned short _id, unsigned short _render_interval);

  virtual void start() {};
  virtual void render() = 0;
  virtual void stop() {};

  void attachController(ScreenController* c);
};

#endif
