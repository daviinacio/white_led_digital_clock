#include "assets/music/base.h"

class BrightnessScreen : public Screen {
private:
  unsigned short cursor = 0;
  unsigned short note = 0;

public:
  BrightnessScreen() : Screen(WLDC_SCREEN_BRIGHTNESS, 250){}

  void start() override {
    display.printScroll(F("    BRILHO    "));
  }

  void render() override {
    display.setCursor(0);
    display.print(F("BR"));

    if(display.getBrightness() < 10)
      display.print(F(" "));
    display.printEnd(display.getBrightness());


    display.setCursor(0);
    display.print(note);
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

    if(key == KEY_VALUE_UP)
      display.incrementBrightness();
    else if(key == KEY_VALUE_DOWN)
      display.decrementBrightness();
    else if(key == KEY_FUNC_LEFT){
      if(note == 0){
        buzzer.tone(NOTE_C3);
      }
      else if(note == 1){
        buzzer.tone(NOTE_D3);
      }
      else if(note == 2){
        buzzer.tone(NOTE_E3);
      }
      else if(note == 3){
        buzzer.tone(NOTE_F3);
      }
      else if(note == 4){
        buzzer.tone(NOTE_G3);
      }
      else if(note == 5){
        buzzer.tone(NOTE_A3);
      }
      else if(note == 6){
        buzzer.tone(NOTE_B3);
      }

      else if(note == 7){
        buzzer.tone(NOTE_C4);
      }
      else if(note == 8){
        buzzer.tone(NOTE_D4);
      }
      else if(note == 9){
        buzzer.tone(NOTE_E4);
      }
      else if(note == 10){
        buzzer.tone(NOTE_F4);
      }
      else if(note == 11){
        buzzer.tone(NOTE_G4);
      }
      else if(note == 12){
        buzzer.tone(NOTE_A4);
      }
      else if(note == 13){
        buzzer.tone(NOTE_B4);
      }

      else if(note == 14){
        buzzer.tone(NOTE_C5);
      }
      else if(note == 15){
        buzzer.tone(NOTE_D5);
      }
      else if(note == 16){
        buzzer.tone(NOTE_E5);
      }
      else if(note == 17){
        buzzer.tone(NOTE_F5);
      }
      else if(note == 18){
        buzzer.tone(NOTE_G5);
      }
      else if(note == 19){
        buzzer.tone(NOTE_A5);
      }
      else if(note == 20){
        buzzer.tone(NOTE_B5);
      }

      else if(note == 21){
        buzzer.tone(NOTE_C6);
      }
      else if(note == 22){
        buzzer.tone(NOTE_D6);
      }
      else if(note == 23){
        buzzer.tone(NOTE_E6);
      }
      else if(note == 24){
        buzzer.tone(NOTE_F6);
      }
      else if(note == 25){
        buzzer.tone(NOTE_G6);
      }
      else if(note == 26){
        buzzer.tone(NOTE_A6);
      }
      else if(note == 27){
        buzzer.tone(NOTE_B6);
      }

      note++;
      note = note % 28;
    }
    else if(key == KEY_FUNC_RIGHT){
      buzzer.stop();
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
