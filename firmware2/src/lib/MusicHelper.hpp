#include "assets/music/base.h"

class MusicHelper {
protected:
  double tunning_a4 = 440.0;
  uint8_t bpm;
  uint8_t beats = 4;
  int8_t octave_shift = 0;
  int8_t transpose_shift = 0;

  double reference_note_frequencies[12];

  double calc_frequency_octave(double frequency_hz, double octave_shift){
    if(octave_shift == 0) return frequency_hz;
    else if(octave_shift > 0) return frequency_hz * (pow(2, abs(octave_shift)));
    else return frequency_hz / (pow(2, abs(octave_shift)));
  }

  double get_oct4_note_frequency(uint8_t note_symbol){
    if(!is_note_symbol(note_symbol)) return 0.0;
    double frequency_hz = reference_note_frequencies[note_symbol - NOTE_A];
    return note_symbol <= NOTE_B ? frequency_hz : calc_frequency_octave(frequency_hz, -1);
  }

  virtual void update_interval() = 0;

public:
  virtual void on_player_finished() = 0;

  void set_tunning_a4(double a4 = 440.0, uint8_t _transpose_shift = 0){
    tunning_a4 = a4;
    transpose_shift = _transpose_shift;
    for (int n = 0; n < 12; ++n){
      reference_note_frequencies[n] = a4 * pow(2.0, (n + transpose_shift) / 12.0);
    }
  };

  void set_bpm(uint8_t _bpm){
    this->bpm = _bpm;
    update_interval();
  };

  void set_beats(uint8_t _beats){
    this->beats = _beats;
    update_interval();
  };

  double get_note_frequency(uint8_t note_symbol, uint8_t octave){
    return calc_frequency_octave(get_oct4_note_frequency(note_symbol), (octave + octave_shift) - 4);
  }

  uint8_t get_timing_ticks(uint8_t timing){
         if(timing ==                WHOLE_NOTE) return WHOLE_NOTE_TIMING_TICKS;
    else if(timing ==                 HALF_NOTE) return WHOLE_NOTE_TIMING_TICKS / 2;
    else if(timing ==              QUARTER_NOTE) return WHOLE_NOTE_TIMING_TICKS / 4;
    else if(timing ==               EIGHTH_NOTE) return WHOLE_NOTE_TIMING_TICKS / 8;
    else if(timing ==            SIXTEENTH_NOTE) return WHOLE_NOTE_TIMING_TICKS / 16;
    else if(timing ==        THIRTY_SECOND_NOTE) return WHOLE_NOTE_TIMING_TICKS / 32;
    else if(timing ==         SIXTY_FOURTH_NOTE) return WHOLE_NOTE_TIMING_TICKS / 64;
    else if(timing ==         WHOLE_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS     ) * 1.5;
    else if(timing ==          HALF_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS /  2) * 1.5;
    else if(timing ==       QUARTER_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS /  4) * 1.5;
    else if(timing ==        EIGHTH_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS /  8) * 1.5;
    else if(timing ==     SIXTEENTH_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS / 16) * 1.5;
    else if(timing == THIRTY_SECOND_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS / 32) * 1.5;
    else if(timing ==  SIXTY_FOURTH_NOTE_DOTTED) return (WHOLE_NOTE_TIMING_TICKS / 64) * 1.5;
    else return 0;
  }

  bool is_note_symbol(uint8_t node){
    return node >= NOTE_A && node <= NOTE_Ab;
  }

  bool is_rest(uint8_t node){
    return node == REST_NOTE;
  }

  bool is_timing(uint8_t node){
    return node >= WHOLE_NOTE && node <= SIXTY_FOURTH_NOTE_DOTTED;
  }

  bool is_timing_modifier(uint8_t node){
    return node == TIE || node == SLUR;
  }

  bool is_valid_value(uint8_t node){
    return node >= MUSIC_MIN_VALUE && node <= MUSIC_MAX_VALUE;
  }
};
