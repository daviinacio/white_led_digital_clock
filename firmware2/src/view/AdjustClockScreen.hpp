#define DISP_BRINK_INTERVAL 300

class AdjustClockScreen : public Screen {
protected:
  unsigned short cursor = 0;
  unsigned short year = 0;
  unsigned short month = 0;
  unsigned short day = 0;
  unsigned short hour = 0;
  unsigned short minute = 0;
  unsigned short second = 0;

public:
  AdjustClockScreen() : Screen(WLDC_SCREEN_ADJUST_CLOCK, 100) {}

  void start() override {
    DateTime now = rtc.now();
    year = now.year();
    month = now.month();
    day = now.day();
    hour = now.hour();
    minute = now.minute();
    
    display.printScroll(F("    CONFIGURACAO    "));
  }

  void render() override {
    bool isIdle = input->isIdle(500);
    display.setCursor(0);
    
    if(cursor == 0 || cursor == 1){  // Minutes & Hours
      display.setCursor(0);
      display.setTimeSeparator(true);

      if((cursor == 1 && millis() / DISP_BRINK_INTERVAL % 3 == 0) && isIdle){   // Blink 1/3 on focus
        display.print(F("  "));
      }
      else {
        if(hour < 10)
          display.print(0);
        display.print(hour);
      }

      if((cursor == 0 && millis()/DISP_BRINK_INTERVAL % 3 == 0) && isIdle){   // Blink 1/3 on focus
        display.print(F("  "));
      }
      else {
        if(minute < 10)
          display.print(0);
        display.print(minute);
      }
    }
    else
    if(cursor == 2){  // Day
      display.setCursor(0);
      display.setTimeSeparator(false);
      display.print(F("D "));

      if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && isIdle){  // Blink 1/3 on focus
        display.print(F("  "));
      }
      else {
        if(day % 100 < 10)
          display.print(F(" "));
          
        display.printEnd(day);
      }
    }
    else
    if(cursor == 3){  // Month
      display.setCursor(0);
      display.print(F("M "));

      if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && isIdle){  // Blink 1/3 on focus
        display.print(F("  "));
      }
      else {
        if(month % 100 < 10)
          display.print(F(" "));
          
        display.printEnd(month);
      }
    }
    else
    if(cursor == 4){  // Year
      display.setCursor(0);
      display.print(F("Y "));

      if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && isIdle){  // Blink 1/3 on focus
        display.print(F("  "));
      }
      else {
        if(year % 100 < 10)
          display.print(F(" "));
          
        display.printEnd((int) year % 100);
      }
    }
  }

  bool keyDown(InputKey key) override {
    if(display.isScrolling()) return true;

    if(key == KEY_FUNC_LEFT)
      increment(cursor, 0, 4, false);
    else if(key == KEY_FUNC_RIGHT)
      decrement(cursor, 0, 4, false);
    else
      handleKey(key);

    render();
    return true;
  }

  void keyPress(InputKey key, unsigned int milliseconds) override {
    if(milliseconds < PANEL_LONG_PRESS) return;
    handleKey(key);
  }

  void handleKey(InputKey key){
    if(cursor == 0) {        // Minute
      if(key == KEY_VALUE_UP){
        increment(minute, 0, 59, true);
      }
      else if(key == KEY_VALUE_DOWN){
        decrement(minute, 0, 59, true);
      }
    }
    else if(cursor == 1) {   // Hour
      if(key == KEY_VALUE_UP){
        increment(hour, 0, 23, true);
      }
      else if(key == KEY_VALUE_DOWN){
        decrement(hour, 0, 23, true);
      }
    }
    else if(cursor == 2) {   // Day
      if(key == KEY_VALUE_UP){
        increment(day, 1, month_last_day(year, month), true);
      }
      else if(key == KEY_VALUE_DOWN){
        decrement(day, 1, month_last_day(year, month), true);
      }
    }
    else if(cursor == 3) {   // Month
      if(key == KEY_VALUE_UP){
        increment(month, 1, 12, true);
      }
      else if(key == KEY_VALUE_DOWN){
        decrement(month, 1, 12, true);
      }

      range(day, 1, month_last_day(year, month));
    }
    else if(cursor == 4) {   // Year
      if(key == KEY_VALUE_UP){
        increment(year, 2000, 2038, true);
      }
      else if(key == KEY_VALUE_DOWN){
        decrement(year, 2000, 2038, true);
      }

      range(day, 1, month_last_day(year, month));
    }
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(!display.isScrolling()){
        rtc.adjust(DateTime(
          year, month, day,
          hour, minute, 0
        ));
        display.printScroll(F("SAVE"), 1500);
        navigate(1);
      }
    }
  }
};
