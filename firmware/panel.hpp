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
  BTN_DEFAULT = 0,
  BTN_HOME = 1,
  BTN_VALUE_UP = 2,
  BTN_VALUE_DOWN = 3,
  BTN_FUNC_LEFT = 4,
  BTN_FUNC_RIGHT = 5,
};

class AnalogPanel : public Thread {
protected:
  int pin;
  long pressing_miliseconds = 0;
  AnalogPanelButton last_button_pressed = AnalogPanelButton::BTN_DEFAULT;
  AnalogPanelButton readPanel();
  void handleEvents(AnalogPanelButton button);
  void (*_onKeyDown)(AnalogPanelButton button);
  void (*_onKeyPress)(AnalogPanelButton button, long milliseconds);
  void (*_onKeyUp)(AnalogPanelButton button, long milliseconds);

public:
  AnalogPanel(int _pin);
  AnalogPanel(int _pin, long _interval);
  void run();
  bool shouldRun(unsigned long time);
  void onKeyDown(void (*callback)(AnalogPanelButton button));
  void onKeyPress(void (*callback)(AnalogPanelButton button, long milliseconds));
  void onKeyUp(void (*callback)(AnalogPanelButton button, long milliseconds));
};

// Implementation
AnalogPanel::AnalogPanel(int _pin, long _interval){
  pin = _pin;
  enabled = false;
  interval = _interval;
}

AnalogPanel::AnalogPanel(int _pin){
  pin = _pin;
  enabled = false;
  interval = PANEL_DEFAULT_INTERVAL;
}

void AnalogPanel::onKeyDown(void (*callback)(AnalogPanelButton)){
  _onKeyDown = callback;
}

void AnalogPanel::onKeyPress(void (*callback)(AnalogPanelButton, long)){
  _onKeyPress = callback;
}

void AnalogPanel::onKeyUp(void (*callback)(AnalogPanelButton, long)){
  _onKeyUp = callback;
}

AnalogPanelButton AnalogPanel::readPanel(){
  AnalogPanelButton button = AnalogPanelButton::BTN_DEFAULT;
  int value = analogRead(pin);

  if(RANGE_BTN_HOME) button = AnalogPanelButton::BTN_HOME;
  else if(RANGE_BTN_VALUE_UP) button = AnalogPanelButton::BTN_VALUE_UP;
  else if(RANGE_BTN_VALUE_DOWN) button = AnalogPanelButton::BTN_VALUE_DOWN;
  else if(RANGE_BTN_FUNC_LEFT) button = AnalogPanelButton::BTN_FUNC_LEFT;
  else if(RANGE_BTN_FUNC_RIGHT) button = AnalogPanelButton::BTN_FUNC_RIGHT;
  
  return button;
}

bool AnalogPanel::shouldRun(unsigned long time){
  AnalogPanelButton button = readPanel();

  if(button != AnalogPanelButton::BTN_DEFAULT){
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
  if(button == AnalogPanelButton::BTN_DEFAULT) {
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
  else if(last_button_pressed == AnalogPanelButton::BTN_DEFAULT){
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

  if(button == AnalogPanelButton::BTN_DEFAULT){
    enabled = false;
  }
  else {
    Thread::run();
  }
}

#endif