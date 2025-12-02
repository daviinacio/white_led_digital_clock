#include <Thread.h>
#include <ThreadController.h>
#include "MusicVoicePlayer.hpp"

#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public Thread, protected MusicHelper {
protected:
  const uint8_t* const* music;
  MusicVoicePlayer* voice_players[BZ_MAX_VOICES];

  void update_interval() override {
    setInterval(((60000.0 / bpm) * beats) / WHOLE_NOTE_TIMING_LOOPS);
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

  void begin(){
    set_tunning_a4();
  }

  const uint8_t* const* getCurrentMusic(){
    return music;
  }

  void playSync(const uint8_t* const* _music, int8_t _octave = 0){
    play(_music, _octave);
    while(enabled)
      if(shouldRun(millis()))
        run();
  }


  void play(const uint8_t* const* _music, int8_t _octave_shift = 0){
    enabled = true;
    octave_shift = _octave_shift;
    
    if(music == _music) return;
    music = _music;

    for (uint8_t i = 0; i < BZ_MAX_VOICES; i++) {
      const uint8_t* voice_pointer = (const uint8_t*) pgm_read_ptr(&_music[i]);
      if(voice_pointer == music_sheet_end) break;
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
      voice_players[i]->run_loop();
    return Thread::run();
  }
};

MusicPlayer player;

#endif
