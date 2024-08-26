#ifndef WLDC_PANEL_H
#define WLDC_PANEL_H
#include "input.hpp"

#define PANEL_DEFAULT_INTERVAL 100

#define RANGE_KEY_HOME        (value < 190)                   // ~0
#define RANGE_KEY_VALUE_UP    (value >= 750 && value < 900)   // ~808
#define RANGE_KEY_VALUE_DOWN  (value >= 550 && value < 750)   // ~651
#define RANGE_KEY_FUNC_LEFT   (value >= 400 && value < 550)   // ~438
#define RANGE_KEY_FUNC_RIGHT  (value >= 190 && value < 400)   // ~205

class AnalogPanel : public Input {
protected:
  unsigned int pin;
  InputKey readPanel();

public:
  AnalogPanel(unsigned int _pin);
  AnalogPanel(unsigned int _pin, unsigned long _interval);
  bool shouldRun(unsigned long time);
};

// Implementation
AnalogPanel::AnalogPanel(unsigned int _pin, unsigned long _interval) : Input(_interval) {
  pin = _pin;
}

AnalogPanel::AnalogPanel(unsigned int _pin) : Input(PANEL_DEFAULT_INTERVAL){
  pin = _pin;
}

InputKey AnalogPanel::readPanel(){
  InputKey key = KEY_DEFAULT;
  unsigned int value = analogRead(pin);

  if(RANGE_KEY_HOME) key = KEY_HOME;
  else if(RANGE_KEY_VALUE_UP) key = KEY_VALUE_UP;
  else if(RANGE_KEY_VALUE_DOWN) key = KEY_VALUE_DOWN;
  else if(RANGE_KEY_FUNC_LEFT) key = KEY_FUNC_LEFT;
  else if(RANGE_KEY_FUNC_RIGHT) key = KEY_FUNC_RIGHT;
  
  return key;
}

bool AnalogPanel::shouldRun(unsigned long time){
  InputKey key = readPanel();

  if(key != KEY_DEFAULT){
    if(!enabled){
      handleEvents(key);
    }
    enabled = true;
  }
  else if(enabled){
    handleEvents(key);
  }

  return Input::shouldRun(time);
}

#endif
