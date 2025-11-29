#include <Thread.h>
#include "assets/music/base.h"

#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public Thread {
protected:
  const uint16_t* music = nullptr;
  float wnd = 0;
  uint16_t gap = 0;
  uint8_t cursor = 0;
  int8_t octave = 0;
  bool is_gap = false;

  float get_whole_note_duration(uint16_t bpm, uint16_t beats){
    return (60000.0 / bpm) * beats;
  }

  uint16_t get_note_duration(uint8_t duration_symbol, float whole_note_duration){
    switch (duration_symbol){
      case WHOLE_NOTE: return whole_note_duration;
      case HALF_NOTE: return whole_note_duration / 2;
      case QUARTER_NOTE: return whole_note_duration / 4;
      case EIGHTH_NOTE: return whole_note_duration / 8;
      case SIXTEENTH_NOTE: return whole_note_duration / 16;
      case THIRTY_SECOND_NOTE: return whole_note_duration / 32;

      case WHOLE_DOTTED_NOTE: return (whole_note_duration) * 1.5; 
      case HALF_DOTTED_NOTE: return (whole_note_duration / 2) * 1.5; 
      case QUARTER_DOTTED_NOTE: return (whole_note_duration / 4) * 1.5; 
      case EIGHTH_DOTTED_NOTE: return (whole_note_duration / 8) * 1.5; 
      case SIXTEENTH_DOTTED_NOTE: return (whole_note_duration / 16) * 1.5; 
      case THIRTY_SECOND_DOTTED_NOTE: return (whole_note_duration / 32) * 1.5; 
    }

    return 0;
  }

  uint16_t calc_note_octave(uint16_t note, int8_t octave){
    if(octave == 0) return note;
    else if(octave > 0) return note * (pow(2, abs(octave)));
    else return note / (pow(2, abs(octave)));
  }

public:
  MusicPlayer(){
    enabled = false;
  }

  void playSync(const uint16_t* _music, int8_t _octave = 0, uint16_t _gap = 32){
    float wnd = get_whole_note_duration(pgm_read_word(&_music[0]), pgm_read_word(&_music[1]));

    for(uint16_t i = 2;; i += 2){
      uint16_t note = pgm_read_word(&_music[i]);
      uint8_t duration = pgm_read_word(&_music[i + 1]);

      if (note == 0 && duration == 0) break;

      buzzer.toneSync(calc_note_octave(note, _octave), get_note_duration(duration, wnd) - _gap);
      delay(_gap);
    }
  }

  void play(const uint16_t* _music, int8_t _octave = 0, uint16_t _gap = 32){
    wnd = get_whole_note_duration(pgm_read_word(&_music[0]), pgm_read_word(&_music[1]));
    music = _music;
    gap = _gap;
    octave = _octave;
    enabled = true;
    cursor = 2;
    setInterval(0);
  }

  void stop(){
    enabled = false;
    music = nullptr;
    buzzer.stop();
  }

  void run() override {
    uint16_t note = pgm_read_word(&music[cursor]);
    uint8_t duration = pgm_read_word(&music[cursor + 1]);

    if (note == 0 && duration == 0) {
      stop();
      return;
    }

    if(is_gap) {
      setInterval(gap);
      is_gap = false;
      buzzer.stop();
      return Thread::run();
    }

    setInterval(get_note_duration(duration, wnd));
    cursor += 2;
    is_gap = true;
    buzzer.tone(calc_note_octave(note, octave));
    return Thread::run();
  }
};

MusicPlayer player;

#endif
