#include <Thread.h>
#include "ScreenController.h"
#include "Input.hpp"

#ifndef WLDC_SCREEN_CLASS_H
#define WLDC_SCREEN_CLASS_H

class ScreenController;

using ScreenID = uint8_t;

class ScreenBase : public Thread, public InputListener {
protected:
  ScreenController* controller = nullptr;
  void navigate(ScreenID screen_id);

  template <typename T>
  void navigate() {
      navigate(T::Id);
  }

  bool isIdle = true;

public:
  virtual void onStart() {};
  virtual void onRender() = 0;
  virtual void onStop() {};

  void attachController(ScreenController* c);
  void run();
};

template <typename ST>
class Screen : public ScreenBase
{
public:
  static constexpr ScreenID kID = ST::Id;
  ScreenID id;

  virtual ~Screen() {}
  Screen() : id(kID) {
    interval = 500;
    ThreadID = id;
  }
};

#endif
