#include "config.h"
#include "lib/Screen.h"

class PomodoroScreen : public Screen, public Thread {
protected:
  uint8_t cursor = 0;

  uint8_t def_work_minutes = 25;
  uint8_t def_short_break_minutes = 5;
  uint8_t def_long_break_minutes = 15;
  uint8_t def_long_break_interval = 4;

  uint16_t remaining_seconds;
  uint8_t current_step;
  bool is_on_break = true;

  void reset(){
    is_on_break = true;
    current_step = 0;
    next_step();
  }

  void next_step(){
    enabled = false;
    is_on_break = !is_on_break;
    if(!is_on_break){
      current_step = (current_step % def_long_break_interval) + 1;
    }

    if(is_on_break){
      remaining_seconds = (
        (current_step == def_long_break_interval) ?
        def_long_break_minutes :
        def_short_break_minutes
      ) * 60;
    }
    else {
      remaining_seconds = def_work_minutes * 60;
    }

  }

public:
  PomodoroScreen() : Screen(WLDC_SCREEN_POMODORO), Thread(NULL, 1000) {
    enabled = false;
    next_step();
  }

  void start() override {
    display.printScroll(F("    POMODORO    "));
  }

  void render() override {
    display.clear();
    display.setTimeSeparator(is_on_break ? remaining_seconds % 2 == 0 : true);

    display.setCursor(0);
      
    if((remaining_seconds / 60) < 10)
      display.print(0);
    display.print((unsigned short) (remaining_seconds / 60));

    if((remaining_seconds % 60) < 10)
      display.print(0);
    display.print((unsigned short) (remaining_seconds % 60));
  }

  bool keyDown(InputKey key) override {
    if(display.isScrolling()) return true;

    if(cursor == 0){
      if(key == KEY_FUNC_LEFT){
        next_step();
      }
      else if(key == KEY_VALUE_UP){
        if(is_on_break && enabled){
          next_step();
        }
        else {
          enabled = !enabled;
        }
      }
    }

    render();
    return true;
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(display.isScrolling())
        navigate(WLDC_SCREEN_CHRONOMETER);
      else
        navigate(WLDC_SCREEN_HOME);
    }
  }

  void run() override {
    remaining_seconds--;
    
    if(remaining_seconds <= 0){
      enabled = false;

      if(is_on_break){
        player.play(fx_alarm0010);
      }
      else {
        player.play(fx_rest0003);
      }

      next_step();
    }
    Thread::run();
  }
};
