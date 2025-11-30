#include <Thread.h>
#include <ThreadController.h>
#include "MusicVoicePlayer.hpp"

#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public ThreadController, protected MusicVoicePlayerObserver {
protected:
  MusicHelper helper;
  const uint16_t* const* music;

  void on_player_finished() override {
    for(int i = 0; i < BZ_MAX_VOICES; i++)
      if(thread[i] && thread[i]->enabled) return;
    
    music = nullptr;
    enabled = false;
  }

public:
  MusicPlayer(){
    enabled = false;

    for(int i = 0; i < BZ_MAX_VOICES; i++){
      MusicVoicePlayer* voice_player = new MusicVoicePlayer(i, &helper);
      voice_player->addObserver(this);
      add(voice_player);
    }
  }

  void playSync(const uint16_t* const* _music, int8_t _octave = 0, uint16_t _gap = 32){
    play(_music, _octave, _gap);
    while(enabled)
      run();
  }

  void play(const uint16_t* const* _music, int8_t _octave = 0, uint16_t _gap = 32){
    helper.read_from_music(_music);
    helper.octave = _octave;
    helper.gap = _gap;
    enabled = true;
    
    if(music == _music) return;
    music = _music;

    for (uint8_t i = 0; i < helper.voice_count; i++) {
      const uint16_t* voice_pointer = (const uint16_t*) pgm_read_ptr(&_music[i + 1]);
      ((MusicVoicePlayer*) thread[i])->play(voice_pointer);
    }
  }

  void pause(){
    enabled = false;
    buzzer.mute();
  }

  void stop(){
    music = nullptr;
    enabled = false;

    for(int i = 0; i < BZ_MAX_VOICES; i++)
      ((MusicVoicePlayer*) thread[i])->stop();
  }
};

MusicPlayer player;

#endif
