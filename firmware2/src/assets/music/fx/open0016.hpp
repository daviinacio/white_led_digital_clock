#include "assets/music/base.h"

static const uint8_t fx_open0016__voice_1[] PROGMEM = {
  MUSIC_BPM,    120,
  MUSIC_BEATS,  4,
  NOTE_C6,  EIGHTH_NOTE,
  NOTE_G5,  EIGHTH_NOTE,
  NOTE_C5,  EIGHTH_NOTE,
  NOTE_F4,  EIGHTH_NOTE,
  NOTE_F5,  EIGHTH_NOTE,
  NOTE_D6,  EIGHTH_NOTE,
  MUSIC_END
};


const uint8_t* const fx_open0016[] PROGMEM = {
  fx_open0016__voice_1,
  music_sheet_end
};
