#include "lib/Screen.h"

const uint16_t* const* music_list[] = {
  sebastian_bach__bourree,
  tv_show__stranger_things_theme,
  coca_cola__theme,
  unknown__la_cucaracha,
  microsoft__windows_xp_shutdown
};

size_t music_list_length = sizeof(music_list) / sizeof(music_list[0]);

class MusicScreen : public Screen {
protected:
  uint8_t cursor = 0;
  uint8_t music_index = 0;
  int8_t octave = 0;

public:
  MusicScreen() : Screen(WLDC_SCREEN_MUSIC, DISP_BRINK_INTERVAL) { }

  void start() override {
    display.printScroll(F("    MUSICA    "));
  }

  void render() override {
    display.clear();
    display.setCursor(0);
    
    if(cursor == 0){
      display.print(F("MS"));

      if((millis()/DISP_BRINK_INTERVAL % 3 == 0) && (music_list[music_index] != player.getCurrentMusic())){
        display.print(F("  "));
      }
      else {
        display.printEnd(music_index + 1);
      }
    }
    else if(cursor == 1){
      display.print(F("OC"));
      display.printEnd(octave);
    }
  }

  void keyUp(InputKey key, unsigned int milliseconds) override {
    if(key == KEY_HOME){
      if(display.isScrolling())
        navigate(WLDC_SCREEN_ADJUST_CLOCK);
      else
        navigate(1);
    }
  }

  bool keyDown(InputKey key) override {
    if(display.isScrolling()) return true;

    if(cursor == 0){
      if(key == KEY_VALUE_UP){
        if(player.enabled)
          player.pause();
        else {
          player.play(music_list[music_index], octave);
        }
      }
      else if(key == KEY_VALUE_DOWN) {
        increment(music_index, 0, music_list_length -1, true);
      }
    }
    else if(cursor == 1){
      if(key == KEY_VALUE_UP){
        increment(octave, -4, 4, false);
      }
      else if(key == KEY_VALUE_DOWN) {
        decrement(octave, -4, 4, false);
      }
    }
    
    if(key == KEY_FUNC_LEFT){
      decrement(cursor, 0, 1, false);
    }
    else if(key == KEY_FUNC_RIGHT){
      increment(cursor, 0, 1, false);
    }

    render();
    return true;
  }

  void keyPress(InputKey key, unsigned int milliseconds) override {
    if(milliseconds < PANEL_LONG_PRESS) return;

    if(key == InputKey::KEY_VALUE_UP){
      player.stop();
      input->release();
      display.printScroll(F("STOP"), 500);
    }
  }

};
