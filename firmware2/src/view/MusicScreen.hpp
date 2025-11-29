#include "lib/Screen.h"

const uint16_t* music_list[] = {
  coca_cola__theme,
  sebastian_bach__bourree,
  unknown__la_cucaracha
};

size_t music_list_length = sizeof(music_list) / sizeof(music_list[0]);

class MusicScreen : public Screen {
protected:
  uint8_t cursor = 0;
  uint8_t music_index = 0;
  int8_t octave = 0;

public:
  MusicScreen() : Screen(WLDC_SCREEN_MUSIC, 500) { }

  void start() override {
    display.printScroll(F("    MUSICA    "));
  }

  void render() override {
    display.clear();
    display.setCursor(0);
    
    if(cursor == 0){
      display.print(F("MS"));
      display.printEnd(music_index + 1);
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
          player.stop();
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

};
