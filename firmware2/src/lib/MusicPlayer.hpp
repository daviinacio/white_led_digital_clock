#include <Thread.h>
#include <ThreadController.h>
#include "MusicVoicePlayer.hpp"

#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public Thread, public MusicHelper {
protected:
  const uint8_t* const* music;
  MusicVoicePlayer* voice_players[BZ_MAX_VOICES];

  void update_interval() override {
    setInterval(((60000.0 / bpm) * beats) / WHOLE_NOTE_TIMING_TICKS);
  }

  void on_player_finished() override {
    for(int i = 0; i < BZ_MAX_VOICES; i++)
      if(voice_players[i] && voice_players[i]->active) return;
    
    stop();
  }

public:
  MusicPlayer() : MusicHelper() {
    enabled = false;
    for(int i = 0; i < BZ_MAX_VOICES; i++){
      voice_players[i] = new MusicVoicePlayer(this, i);
    }
  }

  const uint8_t* const* getCurrentMusic(){
    return music;
  }

  void playSync(const uint8_t* const* _music, int8_t _octave = 0){
    play(_music, _octave);
    while(enabled){
      if(shouldRun(millis()))
        run();

      while(panel.readInput() != InputKey::KEY_DEFAULT){
        buzzer.mute();
      }
    }
  }


  void play(const uint8_t* const* _music, int8_t _octave_shift = 0, int8_t _transpose_shift = 0){
    enabled = true;
    octave_shift = _octave_shift;
    set_tunning_a4(tunning_a4, _transpose_shift);
    
    if(music == _music) return;
    music = _music;

    bool voice_remaining = true;
    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++) {
      voice_players[i]->active = false;

      if(!voice_remaining) continue;
      const uint8_t* voice_pointer = (const uint8_t*) pgm_read_ptr(&_music[i]);
      
      if(voice_pointer == music_sheet_end){
        voice_remaining = false;
        continue;
      }

      voice_players[i]->setup(voice_pointer);
    }
  }

  void pause(){
    enabled = false;
    buzzer.mute();
  }

  void stop(){
    music = nullptr;
    this->pause();
  }

  void run() override {
    for(int i = 0; i < BZ_MAX_VOICES; i++)
      voice_players[i]->run_tick();
    return Thread::run();
  }

  uint16_t calc_music_duration_in_seconds(const uint8_t* const* _music){
    uint16_t cursor = 0;
    uint16_t total_ticks = 0;
    uint8_t bpm = 0;
    uint8_t beats = 0;

    while(true){
      uint8_t node = pgm_read_word(&_music[cursor++]);

      if(node == MUSIC_END)
        break;
      else if(is_timing(node))
        total_ticks += get_timing_ticks(node);
      else if(node == MUSIC_BPM)
        bpm = pgm_read_word(&_music[cursor++]);
      else if(node == MUSIC_BEATS)
        beats = pgm_read_word(&_music[cursor++]);
    }
    
    return (total_ticks / WHOLE_NOTE_TIMING_TICKS) * ((60000.0 / bpm) * beats);
  }
};

MusicPlayer player;

#endif
