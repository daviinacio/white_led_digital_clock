#include <Thread.h>
#include <ThreadController.h>
#include "MusicHelper.hpp"
#include "assets/music/base.h"
#include "drivers/Buzzer.hpp"

#ifndef WLDC_MUSIC_VOICE_PLAYER_H
#define WLDC_MUSIC_VOICE_PLAYER_H

class MusicVoicePlayerObserver {
  public:
    virtual void on_player_finished() = 0;
};

class MusicVoicePlayer : public Thread {
protected:
  MusicVoicePlayerObserver* observer = nullptr; 
  const uint16_t* voice_pointer = nullptr;
  uint8_t voice_index = 0;
  MusicHelper* helper;
  
  bool is_gap = false;
  uint16_t cursor = 0;

  uint16_t repeat_start_position = 0;
  uint16_t repeat_count = 0;

  
public:
  MusicVoicePlayer(
    uint8_t _voice_index,
    MusicHelper* _helper
  ){
    voice_index = _voice_index;
    helper = _helper;
    enabled = false;
  }

  void play(const uint16_t* _voice_pointer){
    voice_pointer = _voice_pointer;
    cursor = 0;
    enabled = true;
    setInterval(0);
  }

  void addObserver(MusicVoicePlayerObserver* _observer){
    observer = _observer;
  }

  void stop(){
    buzzer.mute(voice_index);
    enabled = false;
  }

  void run() override {
    uint16_t marker = pgm_read_word(&voice_pointer[cursor]);
    uint16_t value = pgm_read_word(&voice_pointer[cursor + 1]);

    if(marker == REPEAT_START){
      repeat_count = value;
      cursor += 2;
      repeat_start_position = cursor;
      setInterval(0);
      return;
    }
    else if(marker == REPEAT_END){
      if(repeat_count == 1)
        cursor += 1;
      else {
        repeat_count--;
        cursor = repeat_start_position;
      }
      setInterval(0);
      return;
    }
    
    if (marker == 0 && value == 0) {
      stop();
      if(observer)
      observer->on_player_finished();
      return;
    }
    
    if(is_gap) {
      setInterval(helper->gap);
      is_gap = false;
      buzzer.mute(voice_index);
      return Thread::run();
    }

    if(helper->has_gap(value) && helper->gap != 0)
      is_gap = true;

    setInterval(helper->calc_note_duration(value) - (is_gap ? helper->gap : 0));
    cursor += 2;
    buzzer.tone(helper->calc_note_octave(marker), voice_index);
    return Thread::run();
  }
};

#endif
