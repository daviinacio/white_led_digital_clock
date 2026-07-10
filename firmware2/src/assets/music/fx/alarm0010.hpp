#include "assets/music/base.h"

static const uint8_t fx_alarm0010__voice_1[] PROGMEM = {
  MUSIC_BPM,    120,
  MUSIC_BEATS,  4,
  SIXTY_FOURTH_REST,
  NOTE_Ds7,  EIGHTH_NOTE,
  NOTE_Ds6,  EIGHTH_NOTE,
  NOTE_Ds7,  EIGHTH_NOTE,
  NOTE_Ds6,  EIGHTH_NOTE,
  SIXTY_FOURTH_REST,
  NOTE_Ds7,  EIGHTH_NOTE,
  NOTE_Ds6,  EIGHTH_NOTE,
  MUSIC_END
};


const uint8_t* const fx_alarm0010[] PROGMEM = {
  fx_alarm0010__voice_1,
  music_sheet_end
};
