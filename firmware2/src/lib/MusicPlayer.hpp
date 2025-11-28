#include <Thread.h>
#include "assets/music/base.h"

#ifndef WLDC_MUSIC_PLAYER_H
#define WLDC_MUSIC_PLAYER_H

class MusicPlayer : public Thread {
protected:
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

public:
  MusicPlayer(){
    enabled = false;
  }

  void playSync(const uint16_t* music, uint16_t gap = 32){
    float wnd = get_whole_note_duration(pgm_read_word(&music[0]), pgm_read_word(&music[1]));

    for(uint16_t i = 2;; i += 2){
      uint16_t note = pgm_read_word(&music[i]);
      uint8_t duration = pgm_read_word(&music[i + 1]);

      if (note == 0 && duration == 0) break;

      buzzer.toneSync(note, get_note_duration(duration, wnd) - gap);
      delay(gap);
    }
  }
};

#endif
