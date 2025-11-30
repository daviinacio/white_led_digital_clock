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
  
public:
  // MusicVoicePlayer(
  //   uint8_t _voice_index,
  //   const uint16_t* _voice_pointer,
  //   MusicHelper* _helper
  // ){
  //   Serial.print("MusicVoicePlayer ");
  //   Serial.print("voice_index:");
  //   Serial.print(_voice_index);
  //   Serial.println(";");
    
  //   voice_index = _voice_index;
  //   voice_pointer = _voice_pointer;
  //   helper = _helper;
  // }

  MusicVoicePlayer(
    uint8_t _voice_index,
    MusicHelper* _helper
  ){
    voice_index = _voice_index;
    helper = _helper;
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
    uint16_t note = pgm_read_word(&voice_pointer[cursor]);
    uint8_t duration = pgm_read_word(&voice_pointer[cursor + 1]);
    bool has_gap = !helper->check_tie(duration) && !helper->check_slur(duration) && helper->gap != 0;

    if (note == 0 && duration == 0) {
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

    if(has_gap)
      is_gap = true;

    setInterval(helper->calc_note_duration(duration) - (has_gap ? helper->gap : 0));
    cursor += 2;
    buzzer.tone(helper->calc_note_octave(note), voice_index);
    return Thread::run();
  }
};

#endif
