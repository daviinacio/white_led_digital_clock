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

class Input;

class InputListener {
public:
    virtual ~InputListener() {}
    virtual void keyPress(InputKey key, unsigned int milliseconds){};
    virtual void keyUp(InputKey key, unsigned int milliseconds){};
    virtual bool keyDown(InputKey key){ return true; };

    Input* input = nullptr;
    virtual void attachInput(Input* _input){
      input = _input;
    };
};

class Input : public Thread {
public:
  Input(unsigned long _interval, unsigned short _cooldown);
  void run() override;
  bool shouldRun(unsigned long time);
  void onKeyDown(bool (*callback)(InputKey key));
  void onKeyPress(void (*callback)(InputKey key, unsigned int milliseconds));
  void onKeyUp(void (*callback)(InputKey key, unsigned int milliseconds));
  bool isIdle(unsigned int milliseconds);
  void addEventListener(InputListener* _listener);
protected:
  unsigned long last_time_pressed;
  InputKey last_key_pressed = InputKey::KEY_DEFAULT;
  unsigned short cooldown;
  virtual InputKey readInput() = 0;

  InputListener* listener = nullptr;
  bool (*_onKeyDown)(InputKey key);
  void (*_onKeyPress)(InputKey key, unsigned int milliseconds);
  void (*_onKeyUp)(InputKey key, unsigned int milliseconds);

  void triggerKeyPress(InputKey key, unsigned int milliseconds);
  void triggerKeyUp(InputKey key, unsigned int milliseconds);
  bool triggerKeyDown(InputKey key);
};

// Implementation
inline Input::Input(unsigned long _interval, unsigned short _cooldown): Thread(NULL, _interval){
  enabled = false;
  cooldown = _cooldown;
}

inline void Input::onKeyDown(bool (*callback)(InputKey key)){
  _onKeyDown = callback;
}

inline void Input::onKeyPress(void (*callback)(InputKey key, unsigned int milliseconds)){
  _onKeyPress = callback; 
}

inline void Input::onKeyUp(void (*callback)(InputKey key, unsigned int milliseconds)){
  _onKeyUp = callback; 
}

inline bool Input::shouldRun(unsigned long time){
  InputKey key = readInput();
  if(key == InputKey::KEY_DEFAULT){
    enabled = false;
    if(last_key_pressed != InputKey::KEY_DEFAULT){
      triggerKeyUp(last_key_pressed, millis() - last_time_pressed);
      last_key_pressed = InputKey::KEY_DEFAULT;
      return false;
    }
  }
  else if(last_key_pressed == InputKey::KEY_DEFAULT){
    bool validPress = triggerKeyDown(key);
    last_time_pressed = millis();
    if(validPress){
      last_key_pressed = key;
      enabled = true;
      runned();
      return false;
    }
  }
  return Thread::shouldRun(time);
}

inline void Input::run(){
  triggerKeyPress(last_key_pressed, millis() - last_time_pressed);
  return Thread::run();
}

inline bool Input::isIdle(unsigned int milliseconds){
  return (millis() - last_run) >= milliseconds;
}

inline void Input::addEventListener(InputListener *_listener){
  this->listener = _listener;
  _listener->attachInput(this);
}

inline void Input::triggerKeyPress(InputKey key, unsigned int milliseconds){
  if(_onKeyPress != NULL)
    _onKeyPress(key, milliseconds);

  if(listener) {
    listener->keyPress(key, milliseconds);
  }
}

inline void Input::triggerKeyUp(InputKey key, unsigned int milliseconds){
  if(_onKeyUp != NULL)
    _onKeyUp(last_key_pressed, milliseconds);
  
  if(listener)
    listener->keyUp(last_key_pressed, milliseconds);
}

inline bool Input::triggerKeyDown(InputKey key){
  if(!((millis() - last_time_pressed) >= cooldown)) return false;

  return (
    ((_onKeyDown != NULL) ? _onKeyDown(key) : true) &&
    (listener ? listener->keyDown(key) : true)
  );
}

#endif

