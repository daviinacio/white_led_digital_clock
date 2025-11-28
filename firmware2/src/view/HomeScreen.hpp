class HomeScreen : public Screen {
private:
  uint8_t cursor = 0;

public:
  HomeScreen() : Screen(WLDC_SCREEN_HOME){}

  void start() override {
    cursor = 0;
  }

  void render() override {
    DateTime now = rtc.now();
    bool isIdle = input->isIdle(500);
    unsigned long m = millis();

    // Initial interval
    if(m <= 2000){ /* Do nothing */ }
      
    // Temperature                  // Each 20s, runs on 16s, per 2s
    if(((m / 2000 % 10 == 8 && cursor == 0 && isIdle) || cursor == 1) && dht.hasTemperature()){
      display.setTimeSeparator(false);
      display.setCursor(0);
      display.print((int) dht.getTemperature());
      display.printEnd(F("*C"));
    } else
    
    // Humidity                     // Each 20s, runs on 18s, per 2s
    if(((m / 2000 % 10 == 9 && cursor == 0 && isIdle) || cursor == 2) && dht.hasHumidity()){
      display.setTimeSeparator(false);
      display.clear();
      display.setCursor(0);
      display.print(F("H"));
      display.printEnd(dht.getHumidity());
    }
    
    // Hours and Minutes
    else {
      display.setTimeSeparator(true);
      display.setCursor(0);
      
      if(now.hour() < 10)
        display.print(0);
      display.print(now.hour());

      if(now.minute() < 10)
        display.print(0);
      display.print(now.minute());
    }
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(cursor == 0)
        navigate(WLDC_SCREEN_CHRONOMETER);

      cursor = 0;
      render();
    }
  }

  bool keyDown(InputKey key) override {
    if(key == KEY_VALUE_UP)
      display.incrementBrightness();
    else if(key == KEY_VALUE_DOWN)
      display.decrementBrightness();
    else if(key == InputKey::KEY_FUNC_RIGHT){
      increment(cursor, 0, 2, false);
    }
    else if(key == InputKey::KEY_FUNC_LEFT){
      decrement(cursor, 0, 2, false);
    }

    render();
    return true;
  }

  void keyPress(InputKey key, unsigned int milliseconds){
    if(milliseconds < PANEL_LONG_PRESS) return;
    if(key == InputKey::KEY_VALUE_UP || key == InputKey::KEY_VALUE_DOWN){
      display.autoBrightness();
      display.printScroll(F("AUTO"), 1500);
    }
  }
};
