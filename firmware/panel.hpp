#ifndef WLDC_PANEL_H
#define WLDC_PANEL_H
#include <Thread.h>

#define PANEL_DEFAULT_INTERVAL 100

#define RANGE_BTN_HOME        (value < 190)                   // ~0
#define RANGE_BTN_VALUE_UP    (value >= 750 && value < 900)   // ~808
#define RANGE_BTN_VALUE_DOWN  (value >= 550 && value < 750)   // ~651
#define RANGE_BTN_FUNC_LEFT   (value >= 400 && value < 550)   // ~438
#define RANGE_BTN_FUNC_RIGHT  (value >= 190 && value < 400)   // ~205

enum AnalogPanelButton {
  BTN_DEFAULT = 0x00,
  BTN_HOME = 0x01,
  BTN_VALUE_UP = 0x02,
  BTN_VALUE_DOWN = 0x03,
  BTN_FUNC_LEFT = 0x04,
  BTN_FUNC_RIGHT = 0x05,
};

class AnalogPanel : public Thread {
protected:
  unsigned int pin;
  unsigned int pressing_miliseconds = 0;
  AnalogPanelButton last_button_pressed = BTN_DEFAULT;
  AnalogPanelButton readPanel();
  void handleEvents(AnalogPanelButton button);
  void (*_onKeyDown)(AnalogPanelButton button);
  void (*_onKeyPress)(AnalogPanelButton button, unsigned int milliseconds);
  void (*_onKeyUp)(AnalogPanelButton button, unsigned int milliseconds);

public:
  AnalogPanel(unsigned int _pin);
  AnalogPanel(unsigned int _pin, unsigned long _interval);
  void run();
  bool shouldRun(unsigned long time);
  void onKeyDown(void (*callback)(AnalogPanelButton button));
  void onKeyPress(void (*callback)(AnalogPanelButton button, unsigned int milliseconds));
  void onKeyUp(void (*callback)(AnalogPanelButton button, unsigned int milliseconds));
};

// Implementation
AnalogPanel::AnalogPanel(unsigned int _pin, unsigned long _interval){
  pin = _pin;
  enabled = false;
  interval = _interval;
}

AnalogPanel::AnalogPanel(unsigned int _pin){
  AnalogPanel(_pin, PANEL_DEFAULT_INTERVAL);
}

void AnalogPanel::onKeyDown(void (*callback)(AnalogPanelButton)){
  _onKeyDown = callback;
}

void AnalogPanel::onKeyPress(void (*callback)(AnalogPanelButton, unsigned int)){
  _onKeyPress = callback;
}

void AnalogPanel::onKeyUp(void (*callback)(AnalogPanelButton, unsigned int)){
  _onKeyUp = callback;
}

AnalogPanelButton AnalogPanel::readPanel(){
  AnalogPanelButton button = BTN_DEFAULT;
  int value = analogRead(pin);

  if(RANGE_BTN_HOME) button = BTN_HOME;
  else if(RANGE_BTN_VALUE_UP) button = BTN_VALUE_UP;
  else if(RANGE_BTN_VALUE_DOWN) button = BTN_VALUE_DOWN;
  else if(RANGE_BTN_FUNC_LEFT) button = BTN_FUNC_LEFT;
  else if(RANGE_BTN_FUNC_RIGHT) button = BTN_FUNC_RIGHT;
  
  return button;
}

bool AnalogPanel::shouldRun(unsigned long time){
  AnalogPanelButton button = readPanel();

  if(button != BTN_DEFAULT){
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

void AnalogPanel::run() {
  AnalogPanelButton button = readPanel();
  handleEvents(button);
}

void AnalogPanel::handleEvents(AnalogPanelButton button){
  // Kew Up
  if(button == BTN_DEFAULT) {
    if(_onKeyUp != NULL)
      _onKeyUp(last_button_pressed, pressing_miliseconds);
    pressing_miliseconds = 0;
  }
  // Key Press
  else if(button == last_button_pressed){
    pressing_miliseconds += interval;
    if(_onKeyPress != NULL)
      _onKeyPress(button, pressing_miliseconds);
  }
  // Key Down
  else if(last_button_pressed == BTN_DEFAULT){
    if(_onKeyDown != NULL)
      _onKeyDown(button);
  }
  // Changed Button
  else {
    if(_onKeyUp != NULL)
      _onKeyUp(last_button_pressed, pressing_miliseconds);
    pressing_miliseconds = 0;

    if(_onKeyDown != NULL)
      _onKeyDown(button);
  }

  last_button_pressed = button;

  if(button == BTN_DEFAULT){
    enabled = false;
  }
  else {
    Thread::run();
  }
}

#endif
