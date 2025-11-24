#ifndef WLDC_PANEL_H
#define WLDC_PANEL_H
#include "../lib/Input.hpp"

#define PANEL_DEFAULT_INTERVAL 100
#define PANEL_DEFAULT_COOLDOWN 150

#define RANGE_KEY_HOME        (value < 190)                   // ~0
#define RANGE_KEY_VALUE_UP    (value >= 750 && value < 900)   // ~808
#define RANGE_KEY_VALUE_DOWN  (value >= 550 && value < 750)   // ~651
#define RANGE_KEY_FUNC_LEFT   (value >= 400 && value < 550)   // ~438
#define RANGE_KEY_FUNC_RIGHT  (value >= 190 && value < 400)   // ~205

class AnalogPanel : public Input {
protected:
  unsigned short pin;
  unsigned short cooldown;
  InputKey readInput() override;
  void handleEvents(InputKey button) override;

public:
  AnalogPanel(unsigned short _pin);
  AnalogPanel(unsigned short _pin, unsigned long _interval);
  AnalogPanel(unsigned short _pin, unsigned long _interval, unsigned short _cooldown);
};

// Implementation
AnalogPanel::AnalogPanel(unsigned short _pin, unsigned long _interval, unsigned short _cooldown) : Input(_interval) {
  pin = _pin;
  cooldown = _cooldown;
}

AnalogPanel::AnalogPanel(unsigned short _pin, unsigned long _interval) : Input(_interval) {
  pin = _pin;
  cooldown = PANEL_DEFAULT_COOLDOWN;
}

AnalogPanel::AnalogPanel(unsigned short _pin) : Input(PANEL_DEFAULT_INTERVAL){
  pin = _pin;
  cooldown = PANEL_DEFAULT_COOLDOWN;
}

InputKey AnalogPanel::readInput() {
  InputKey key = KEY_DEFAULT;
  unsigned int value = analogRead(pin);

  if(RANGE_KEY_HOME) key = KEY_HOME;
  else if(RANGE_KEY_VALUE_UP) key = KEY_VALUE_UP;
  else if(RANGE_KEY_VALUE_DOWN) key = KEY_VALUE_DOWN;
  else if(RANGE_KEY_FUNC_LEFT) key = KEY_FUNC_LEFT;
  else if(RANGE_KEY_FUNC_RIGHT) key = KEY_FUNC_RIGHT;
  
  return key;
}

inline void AnalogPanel::handleEvents(InputKey button){
  bool isIdle = this->isIdle(cooldown);
  if(!isIdle && button != KEY_DEFAULT) return;
  Input::handleEvents(button);
}

#endif
