#include <Thread.h>
#include <ThreadController.h>
#include "MusicVoicePlayer.hpp"


#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public ThreadController, protected MusicVoicePlayerObserver {
protected:
  MusicHelper helper;
  const uint16_t* const* music;

public:
  MusicPlayer(){
    enabled = false;

    for(int i = 0; i < BZ_MAX_VOICES; i++){
      MusicVoicePlayer* voice_player = new MusicVoicePlayer(i, &helper);
      voice_player->addObserver(this);
      add(voice_player);
    }
  }

  // void clear(){
  //   for(int i = 0; i < MAX_THREADS; i++){
  //     thread[i] = NULL;
  //   }
  //   cached_size = 0;
  // }

  // void playSync(const uint16_t** _music, int8_t _octave = 0, uint16_t _gap = 32){
  //   read_music_attributes(_music);
  //   float wnd = get_whole_note_duration(pgm_read_word(&_music[0]), pgm_read_word(&_music[1]));

  //   for(uint16_t i = 2;; i += 2){
  //     uint16_t note = pgm_read_word(&_music[i]);
  //     uint8_t duration = pgm_read_word(&_music[i + 1]);
  //     bool has_gap = !check_tie(duration) && !check_slur(duration);

  //     if (note == 0 && duration == 0) {
  //       buzzer.mute();
  //       break;
  //     }

  //     buzzer.tone(calc_note_octave(note, _octave), 0);
  //     delay(get_note_duration(duration, wnd) - (has_gap ? _gap : 0));

  //     if(has_gap)
  //       delay(_gap);
  //   }
  // }

  void play(const uint16_t* const* _music, int8_t _octave = 0, uint16_t _gap = 32){
    helper.read_from_music(_music);
    helper.octave = _octave;
    helper.gap = _gap;

    enabled = true;
    
    if(music == _music) return;
    stop();
    music = _music;

    for (uint8_t i = 0; i < helper.voice_count; i++) {
      const uint16_t* voice_pointer = (const uint16_t*) pgm_read_ptr(&_music[i + 1]);
      ((MusicVoicePlayer*) thread[i])->play(voice_pointer);
    }

    enabled = true;
  }

  void pause(){
    enabled = false;
    buzzer.mute();
  }

  void stop(){
    music = nullptr;
    enabled = false;

    for(Thread* thr : thread)
      ((MusicVoicePlayer*) thr)->stop();
  }

  void on_player_finished() override {
    for(Thread* thr : thread)
      if(thr && thr->enabled) return;
    stop();
  }
};

MusicPlayer player;

#endif
