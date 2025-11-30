#include "assets/music/base.h"

class MusicHelper {
public:
  // ROM attributes
  uint16_t bpm = 0;
  uint16_t beats = 0;
  uint16_t voice_count = 0;

  // Runtime attributes
  uint16_t gap = 0;
  int8_t octave = 0;

  void read_from_music(const uint16_t* const* _music){
    const uint16_t* metadata = (const uint16_t*) pgm_read_ptr(&_music[0]);
    bpm = pgm_read_word(&metadata[0]);
    beats = pgm_read_word(&metadata[1]);
    voice_count = pgm_read_word(&metadata[2]);
  }

  void clear(){
    gap = 0;
    octave = 0;
    bpm = 0;
    beats = 0;
    voice_count = 0;
  }

  float whole_note_duration(){
    return (60000.0 / bpm) * beats;
  }

  uint16_t calc_note_duration(uint8_t duration_symbol){
    float whole_note_duration = this->whole_note_duration();

    if(
      (duration_symbol == WHOLE_NOTE) ||
      (duration_symbol == WHOLE_NOTE_TIE) ||
      (duration_symbol == WHOLE_NOTE_SLUR)
    ) return whole_note_duration;
    else if(
      (duration_symbol == HALF_NOTE) ||
      (duration_symbol == HALF_NOTE_TIE) ||
      (duration_symbol == HALF_NOTE_SLUR)
    ) return whole_note_duration / 2;
    else if(
      (duration_symbol == QUARTER_NOTE) ||
      (duration_symbol == QUARTER_NOTE_TIE) ||
      (duration_symbol == QUARTER_NOTE_SLUR)
    ) return whole_note_duration / 4;
    else if(
      (duration_symbol == EIGHTH_NOTE) ||
      (duration_symbol == EIGHTH_NOTE_TIE) ||
      (duration_symbol == EIGHTH_NOTE_SLUR)
    ) return whole_note_duration / 8;
    else if(
      (duration_symbol == SIXTEENTH_NOTE) ||
      (duration_symbol == SIXTEENTH_NOTE_TIE) ||
      (duration_symbol == SIXTEENTH_NOTE_SLUR)
    ) return whole_note_duration / 16;
    else if(
      (duration_symbol == THIRTY_SECOND_NOTE) ||
      (duration_symbol == THIRTY_SECOND_NOTE_TIE) ||
      (duration_symbol == THIRTY_SECOND_NOTE_SLUR)
    ) return whole_note_duration / 32;

    else if(
      (duration_symbol == WHOLE_NOTE_DOTTED)
    ) return (whole_note_duration) * 1.5; 
    else if(
      (duration_symbol == HALF_NOTE_DOTTED)
    ) return (whole_note_duration / 2) * 1.5; 
    else if(
      (duration_symbol == QUARTER_NOTE_DOTTED)
    ) return (whole_note_duration / 4) * 1.5; 
    else if(
      (duration_symbol == EIGHTH_NOTE_DOTTED)
    ) return (whole_note_duration / 8) * 1.5; 
    else if(
      (duration_symbol == SIXTEENTH_NOTE_DOTTED)
    ) return (whole_note_duration / 16) * 1.5; 
    else if(
      (duration_symbol == THIRTY_SECOND_NOTE_DOTTED)
    ) return (whole_note_duration / 32) * 1.5;

    return 0;
  }

    bool check_tie(uint8_t duration_symbol){
    return (
      duration_symbol == WHOLE_NOTE_TIE ||
      duration_symbol == HALF_NOTE_TIE ||
      duration_symbol == QUARTER_NOTE_TIE ||
      duration_symbol == EIGHTH_NOTE_TIE ||
      duration_symbol == SIXTEENTH_NOTE_TIE ||
      duration_symbol == THIRTY_SECOND_NOTE_TIE
    );
  }

  bool check_slur(uint8_t duration_symbol){
    return (
      duration_symbol == WHOLE_NOTE_SLUR ||
      duration_symbol == HALF_NOTE_SLUR ||
      duration_symbol == QUARTER_NOTE_SLUR ||
      duration_symbol == EIGHTH_NOTE_SLUR ||
      duration_symbol == SIXTEENTH_NOTE_SLUR ||
      duration_symbol == THIRTY_SECOND_NOTE_SLUR
    );
  }

  uint16_t calc_note_octave(uint16_t note){
    if(octave == 0) return note;
    else if(octave > 0) return note * (pow(2, abs(octave)));
    else return note / (pow(2, abs(octave)));
  }
};
