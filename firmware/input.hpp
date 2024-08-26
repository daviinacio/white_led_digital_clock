#ifndef WLDC_INPUT_H
#define WLDC_INPUT_H
#include <Thread.h>

enum InputKey {
  KEY_DEFAULT = 0x00,
  KEY_HOME = 0x01,
  KEY_VALUE_UP = 0x02,
  KEY_VALUE_DOWN = 0x03,
  KEY_FUNC_LEFT = 0x04,
  KEY_FUNC_RIGHT = 0x05,
};

class Input : public Thread {
protected:
  unsigned int pressing_miliseconds = 0;
  InputKey last_key_pressed = KEY_DEFAULT;
  virtual InputKey readPanel();
  void handleEvents(InputKey key);
  void (*_onKeyDown)(InputKey key);
  void (*_onKeyPress)(InputKey key, unsigned int milliseconds);
  void (*_onKeyUp)(InputKey key, unsigned int milliseconds);

public:
  Input(unsigned long _interval);
  void run();
  bool shouldRun(unsigned long time);
  void onKeyDown(void (*callback)(InputKey key));
  void onKeyPress(void (*callback)(InputKey key, unsigned int milliseconds));
  void onKeyUp(void (*callback)(InputKey key, unsigned int milliseconds));
};

// Implementation
Input::Input(unsigned long _interval){
  enabled = false;
  interval = _interval;
}

void Input::onKeyDown(void (*callback)(InputKey)){
  _onKeyDown = callback;
}

void Input::onKeyPress(void (*callback)(InputKey, unsigned int)){
  _onKeyPress = callback;
}

void Input::onKeyUp(void (*callback)(InputKey, unsigned int)){
  _onKeyUp = callback;
}

bool Input::shouldRun(unsigned long time){
  InputKey button = readPanel();

  if(button != KEY_DEFAULT){
    if(!enabled){
      handleEvents(button);
    }
    enabled = true;
  }
  else if(enabled){
    handleEvents(button);
  }

  return Thread::shouldRun(time);
}

void Input::run() {
  InputKey button = readPanel();
  handleEvents(button);
}

void Input::handleEvents(InputKey button){
  // Kew Up
  if(button == KEY_DEFAULT) {
    if(_onKeyUp != NULL)
      _onKeyUp(last_key_pressed, pressing_miliseconds);
    pressing_miliseconds = 0;
  }
  // Key Press
  else if(button == last_key_pressed){
    pressing_miliseconds += interval;
    if(_onKeyPress != NULL)
      _onKeyPress(button, pressing_miliseconds);
  }
  // Key Down
  else if(last_key_pressed == KEY_DEFAULT){
    if(_onKeyDown != NULL)
      _onKeyDown(button);
  }
  // Changed Button
  else {
    if(_onKeyUp != NULL)
      _onKeyUp(last_key_pressed, pressing_miliseconds);
    pressing_miliseconds = 0;

    if(_onKeyDown != NULL)
      _onKeyDown(button);
  }

  last_key_pressed = button;

  if(button == KEY_DEFAULT){
    enabled = false;
  }
  else {
    Thread::run();
  }
}

#endif