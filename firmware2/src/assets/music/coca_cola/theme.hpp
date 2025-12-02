#include "assets/music/base.h"

const uint8_t coca_cola__theme__voice_1[] PROGMEM = {
  MUSIC_BPM,    112,
  MUSIC_BEATS,  2,

  NOTE_E5,  QUARTER_NOTE,
  NOTE_C5,  QUARTER_NOTE,
  NOTE_D5,  QUARTER_NOTE,
  NOTE_E5,  EIGHTH_NOTE,
  NOTE_C5,  QUARTER_NOTE,

  MUSIC_END
};

const uint8_t coca_cola__theme__voice_2[] PROGMEM = {
  NOTE_E4,  QUARTER_NOTE,
  NOTE_C4,  QUARTER_NOTE,
  NOTE_D4,  QUARTER_NOTE,
  NOTE_E4,  EIGHTH_NOTE,
  NOTE_C4,  QUARTER_NOTE,

  MUSIC_END
};


const uint8_t* const coca_cola__theme[] PROGMEM = {
  coca_cola__theme__voice_1,
  coca_cola__theme__voice_2,
  music_sheet_end
};
