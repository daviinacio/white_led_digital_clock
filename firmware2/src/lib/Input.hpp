#include <Thread.h>

#ifndef WLDC_INPUT_H
#define WLDC_INPUT_H

enum InputKey {
  KEY_DEFAULT = 0x00,
  KEY_HOME = 0x01,
  KEY_VALUE_UP = 0x02,
  KEY_VALUE_DOWN = 0x03,
  KEY_FUNC_LEFT = 0x04,
  KEY_FUNC_RIGHT = 0x05,
};

class InputListener {
public:
    virtual ~InputListener() {}
    virtual void onKeyPress(InputKey key, unsigned int milliseconds){};
    virtual void onKeyUp(InputKey key, unsigned int milliseconds){};
    virtual bool onKeyDown(InputKey key){ return false; };
};

class Input : public Thread {
protected:
  unsigned int pressing_miliseconds = 0;
  InputKey last_key_pressed = KEY_DEFAULT;
  virtual InputKey readInput() = 0;
  virtual void handleEvents(InputKey key);

  InputListener* listener = nullptr;
  // bool (*_onKeyDown)(InputKey key);
  // void (*_onKeyPress)(InputKey key, unsigned int milliseconds);
  // void (*_onKeyUp)(InputKey key, unsigned int milliseconds);

public:
  Input(unsigned long _interval);
  void run();
  bool shouldRun(unsigned long time);
  // void onKeyDown(bool (*callback)(InputKey key));
  // void onKeyPress(void (*callback)(InputKey key, unsigned int milliseconds));
  // void onKeyUp(void (*callback)(InputKey key, unsigned int milliseconds));
  bool isIdle(unsigned int milliseconds);
  void addEventListener(InputListener* _listener);
};

// Implementation
inline Input::Input(unsigned long _interval){
  enabled = false;
  interval = _interval;
}

// inline void Input::onKeyDown(bool (*callback)(InputKey key)){
//   _onKeyDown = callback;
// }

// inline void Input::onKeyPress(void (*callback)(InputKey key, unsigned int milliseconds)){
//   _onKeyPress = callback; 
// }

// inline void Input::onKeyUp(void (*callback)(InputKey key, unsigned int milliseconds)){
//   _onKeyUp = callback; 
// }

inline bool Input::shouldRun(unsigned long time){
  InputKey button = readInput();

  if(button != KEY_DEFAULT){
    if(!enabled){
      // runned();
      enabled = true;
      return true;
    }
  }
  else if(enabled){
    handleEvents(button);
    runned();
  }

  return Thread::shouldRun(time);
}

inline void Input::run()
{
  InputKey button = readInput();
  handleEvents(button);
  runned();
}

inline void Input::handleEvents(InputKey button){

  // Kew Up
  if(button == KEY_DEFAULT && last_key_pressed != KEY_DEFAULT) {
    // if(_onKeyUp != NULL)
    //   _onKeyUp(last_key_pressed, pressing_miliseconds);

    if(listener)
      listener->onKeyUp(last_key_pressed, pressing_miliseconds);
    pressing_miliseconds = 0;
  }
  // Key Press
  else if(button == last_key_pressed){
    pressing_miliseconds += interval;
    // if(_onKeyPress != NULL)
    //   _onKeyPress(button, pressing_miliseconds);

    if(listener) {
      listener->onKeyPress(button, pressing_miliseconds);
    }
  }
  // Key Down
  else if(last_key_pressed == KEY_DEFAULT){
    // if(_onKeyDown != NULL) {
    //   if(!_onKeyDown(button)) {
    //     // Bypass key down
    //     button = KEY_DEFAULT;
    //   }
    // }

    if(listener) {
      if(!listener->onKeyDown(button)) {
        // Bypass key down
        button = KEY_DEFAULT;
      }
    }
  }
  // Changed Button
  else {
    // if(_onKeyUp != NULL)
    //   _onKeyUp(last_key_pressed, pressing_miliseconds);
    
    if(listener)
      listener->onKeyUp(last_key_pressed, pressing_miliseconds);

    pressing_miliseconds = 0;

    // if(_onKeyDown != NULL)
    //   _onKeyDown(button);

    if(listener) {
      listener->onKeyDown(button);
    }
  }

  last_key_pressed = button;

  if(button == KEY_DEFAULT){
    enabled = false;
  }
}

inline bool Input::isIdle(unsigned int milliseconds){
  return (millis() - milliseconds) >= last_run;
}

inline void Input::addEventListener(InputListener *_listener)
{
  this->listener = _listener;
}

#endif

